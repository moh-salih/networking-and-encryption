#include <iostream>
#include "net/client/Client.h"

using net::protocol::Message;
using net::protocol::MessageType;
using net::protocol::kHeaderSize;
using net::protocol::kTypeFieldSize;
using net::protocol::kLengthFieldSize;
using net::protocol::json;
using boost::asio::ip::tcp;

namespace net::client {
    
    Client::Client(boost::asio::io_context& io, std::unique_ptr<ITransport> transport, RunMode mode)
        : mIoContext(io), mTransport(std::move(transport)), mRunMode(mode) {
    }

    Client::~Client() {
        close();
    }

    bool Client::isRunning() const {
        return mIsRunning;
    }

    // --- Event Handlers ---
    void Client::onConnect(VoidCallback handler) { mConnectCallback = std::move(handler); }
    void Client::onDisconnect(VoidCallback handler) { mDisconnectCallback = std::move(handler); }
    void Client::onError(ErrorCallback handler) { mErrorCallback = std::move(handler); }
    void Client::onPush(PushHandler handler) { mPushCallback = std::move(handler); }

    bool Client::connect(const std::string& host, uint16_t port) {
        if (mIsRunning.exchange(true)) return false;

        mWorkGuard = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(mIoContext.get_executor());

        mTransport->connect(host, port, [this](const boost::system::error_code& ec) {
            handleConnectResult(ec);
        });
        
        if (mRunMode == RunMode::Threaded) {
            mThread = std::thread([this] { 
                try {
                    mIoContext.run(); 
                } catch (...) {
                    mIsRunning = false;
                }
            });
        }

        return true;
    }

    void Client::handleConnectResult(const boost::system::error_code& ec) {
        if (ec) {
            mIsRunning = false;
            if (mErrorCallback) mErrorCallback(ec.message());
            if (mDisconnectCallback) mDisconnectCallback();
            return;
        }

        if (mConnectCallback) mConnectCallback();
        readHeader();
    }

    void Client::poll() {
        if (mRunMode == RunMode::Manual) {
            // Legacy uses simple poll() + restart()
            mIoContext.poll();
            mIoContext.restart();
        }
    }

    bool Client::handleIoError(const boost::system::error_code& ec) {
        if (!ec) return false;
        if (mIsRunning) {
            if (mErrorCallback) mErrorCallback(ec.message());
            close();
        }
        return true;
    }

    void Client::requestAsync(const std::string& method, const json& params, ResponseCallback callback) {
        if (!mIsRunning) return;

        uint32_t id = mNextRequestId.fetch_add(1);
        {
            std::lock_guard<std::mutex> lock(mCallbackMutex);
            mResponseCallbacks[id] = std::move(callback);
        }

        writeMessage(Message::makeRequest(id, method, params));
    }

    json Client::request(const std::string& method, const json& params) {
        if (!mIsRunning) throw std::runtime_error("Client not connected");

        std::promise<json> promise;
        auto future = promise.get_future();

        requestAsync(method, params, [&promise](const json& result) {
            promise.set_value(result);
        });

        return future.get();
    }

    void Client::writeMessage(const net::protocol::Message& msg) {
        auto bytes = std::make_shared<std::vector<uint8_t>>(msg.encode());
        // Use post to ensure the transport doesn't block the caller
        boost::asio::post(mIoContext, [this, bytes]() {
            mTransport->asyncWrite(boost::asio::buffer(*bytes), [this, bytes](auto ec, size_t) { 
                handleIoError(ec); 
            });
        });
    }

    void Client::readHeader() {
        // Match legacy: resize buffer to header size
        mReadBuffer.resize(kHeaderSize);
        mTransport->asyncRead(boost::asio::buffer(mReadBuffer), [this](auto ec, std::size_t) {
                if (handleIoError(ec)) return;

                uint32_t lenNet;
                std::memcpy(&lenNet, &mReadBuffer[kTypeFieldSize], kLengthFieldSize);
                uint32_t payloadSize = ntohl(lenNet);
                
                readBody(payloadSize);
            }
        );
    }

    void Client::readBody(size_t payloadSize) {
        // Match legacy: resize to total size (header + body)
        mReadBuffer.resize(kHeaderSize + payloadSize);

        mTransport->asyncRead(
            boost::asio::buffer(mReadBuffer.data() + kHeaderSize, payloadSize),
            [this, payloadSize](auto ec, std::size_t) {
                if (handleIoError(ec)) return;

                try {
                    MessageType type = static_cast<MessageType>(mReadBuffer[0]);
                    
                    // --- THE FIX: SAFE ITERATOR HANDLING ---
                    // Create a stack-local copy of the payload bytes.
                    // This ensures that 'Message::decode' (and the JSON parser) 
                    // holds iterators to this local vector, NOT the member mReadBuffer.
                    // If mReadBuffer is resized by the next readHeader cycle, 
                    // these local iterators remain valid.
                    std::vector<uint8_t> payload(mReadBuffer.begin() + kHeaderSize, mReadBuffer.end());
                    
                    Message msg = Message::decode(type, payload);
                    handleMessage(msg);
                    
                } catch (const std::exception& e) {
                    if (mErrorCallback) mErrorCallback("Decode error: " + std::string(e.what()));
                    close();
                    return;
                }

                // Start next read after processing current message
                if (mIsRunning) {
                    readHeader();
                }
            }
        );
    }

    void Client::handleMessage(const Message& msg) {
        if (msg.type == MessageType::Response) {
            uint32_t id = msg.j.value("id", 0);
            ResponseCallback callback = nullptr;

            {
                std::lock_guard<std::mutex> lock(mCallbackMutex);
                auto it = mResponseCallbacks.find(id);
                if (it != mResponseCallbacks.end()) {
                    callback = std::move(it->second);
                    mResponseCallbacks.erase(it);
                }
            }

            if (callback) {
                if (msg.j.contains("error")) {
                    callback(msg.j["error"]);
                } else {
                    callback(msg.j.value("result", json::object()));
                }
            }
        } else if (msg.type == MessageType::Push) {
            if (mPushCallback) {
                mPushCallback(msg.j.value("push", json::object()));
            }
        }
    }

    void Client::stopIo() {
        mWorkGuard.reset();
        if (!mIoContext.stopped()) {
            mIoContext.stop();
        }
        if (mThread.joinable()) {
            mThread.join();
        }
        mIoContext.restart();
    }

    void Client::close() {
        if (!mIsRunning.exchange(false)) return;

        // Post disconnect to context to maintain order
        boost::asio::post(mIoContext, [this]() {
            if (mDisconnectCallback) mDisconnectCallback();
        });

        if (mTransport) {
            mTransport->close();
        }

        stopIo();
    }

} // namespace net::client