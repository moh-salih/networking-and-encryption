#include "net/server/sessions/PlainSession.h"
#include <iostream>

using net::protocol::Message;
using net::protocol::MessageType;
using net::protocol::json;

namespace net::server::sessions {

    PlainSession::PlainSession(TcpSocket&& socket): mSocket(std::move(socket)){}

    void PlainSession::start(){
        if(mStartSessionCallback) mStartSessionCallback(shared_from_this());
        readHeader();
    }
    
    void PlainSession::onStart(const SessionCallback& handler){
        mStartSessionCallback = handler;
    }
    
    void PlainSession::close(){
        if (mIsClosed.exchange(true)) {
            return; // Already closing
        }

        boost::system::error_code ec;
        mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        mSocket.close(ec);

        if(mCloseSessionCallback) mCloseSessionCallback(shared_from_this());
    }
    
    void PlainSession::onClose(const SessionCallback& handler){
        mCloseSessionCallback = handler;
    }
    
    void PlainSession::send(const net::protocol::Message& message) {
        write(message);
    }

    void PlainSession::onMessage(const MessageCallback& handler) {
        mMessageCallback = handler;
    }

    void PlainSession::onError(const ErrorCallback& handler) {
        mErrorCallback = handler;
    }

    void PlainSession::setUid(uint32_t uid) {
        mUid = uid;
    }

    uint32_t PlainSession::getUid() const {
        return mUid;
    }

    void PlainSession::read() {
        readHeader();
    }
    

    void PlainSession::readBody(std::size_t payloadSize) {
        auto self = shared_from_this();
        mBuffer.resize(net::protocol::kHeaderSize + payloadSize);

        boost::asio::async_read(mSocket, boost::asio::buffer(mBuffer.data() + net::protocol::kHeaderSize, payloadSize),
            [this, self, payloadSize](auto ec, std::size_t) {
                onBodyRead(ec, payloadSize);
            }
        );
    }

    void PlainSession::onBodyRead(const boost::system::error_code& ec, std::size_t payloadSize){
        auto self = shared_from_this();
        if (ec) {
            if(mErrorCallback) mErrorCallback(ec.message(), self);
            return close();
        }

        MessageType type = static_cast<MessageType>(mBuffer[0]);
        std::vector<uint8_t> payload(mBuffer.begin() + net::protocol::kHeaderSize, mBuffer.end());

        try{
            if(mMessageCallback) {
                Message message = Message::decode(type, payload);
                mMessageCallback(message, self);
            }
        }catch(const std::exception& e){
            if(mErrorCallback) mErrorCallback(std::string(e.what()), self);
            return close(); 
        }
        readHeader(); // Start reading the next message
    }


    void PlainSession::readHeader() {
        auto self = shared_from_this();
        mBuffer.resize(net::protocol::kHeaderSize);

        boost::asio::async_read(
            mSocket,
            boost::asio::buffer(mBuffer),
            [this, self](auto ec, std::size_t) {
                onHeaderRead(ec);
            }
        );
    }

    void PlainSession::onHeaderRead(const boost::system::error_code& ec) {
        if (ec) {
            if (mErrorCallback) mErrorCallback(ec.message(), shared_from_this());
            return close();
        }

        uint32_t lenNet;
        std::memcpy(&lenNet, &mBuffer[net::protocol::kTypeFieldSize],
                    net::protocol::kLengthFieldSize);

        std::size_t payloadSize = ntohl(lenNet);

        constexpr std::size_t MAX_PAYLOAD = 1024 * 1024;
        if (payloadSize > MAX_PAYLOAD) {
            if (mErrorCallback)
                mErrorCallback("Payload too large", shared_from_this());
            return close();
        }

        readBody(payloadSize);
    }

    void PlainSession::write(const net::protocol::Message& message) {
        auto self = shared_from_this();
        auto bytes = std::make_shared<std::vector<uint8_t>>(message.encode()); // used shared pointer here to keep it alive

        // The below prevents concurrent writes from effecting each other.
        boost::asio::post(mSocket.get_executor(), [this, self, bytes]{
            boost::asio::async_write(mSocket, boost::asio::buffer(*bytes), [this, self, bytes](auto ec, size_t){
                if(ec) {
                    if(mErrorCallback) mErrorCallback(ec.message(), self);
                    boost::asio::post(mSocket.get_executor(), [this, self]{
                        close();
                    });
                }
            });
        });
    }

} // namespace net::server::sessions

