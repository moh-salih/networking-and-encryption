#include "net/server/sessions/SecureSession.h"

using net::protocol::Message;
using net::protocol::MessageType;

namespace net::server::sessions {

    SecureSession::SecureSession(TcpSocket&& socket, boost::asio::ssl::context& sslCtx)
        : mStream(std::move(socket), sslCtx) {}

    void SecureSession::start() {
        doHandshake();
    }

    void SecureSession::doHandshake() {
        auto self = shared_from_this();

        mStream.async_handshake(
            boost::asio::ssl::stream_base::server,
            [this, self](const boost::system::error_code& ec) {
                if (ec) {
                    if (mErrorCallback) mErrorCallback(ec.message(), self);
                    return close();
                }

                if (mStartSessionCallback) mStartSessionCallback(self);
                readHeader();
            }
        );
    }

    void SecureSession::onStart(const SessionCallback& handler) {
        mStartSessionCallback = handler;
    }

    void SecureSession::close() {
        if (mIsClosed.exchange(true)) return;

        boost::system::error_code ec;
        mStream.shutdown(ec);
        mStream.next_layer().close(ec);

        if (mCloseSessionCallback)
            mCloseSessionCallback(shared_from_this());
    }

    void SecureSession::onClose(const SessionCallback& handler) {
        mCloseSessionCallback = handler;
    }

    void SecureSession::send(const Message& message) {
        write(message);
    }

    void SecureSession::onMessage(const MessageCallback& handler) {
        mMessageCallback = handler;
    }

    void SecureSession::onError(const ErrorCallback& handler) {
        mErrorCallback = handler;
    }

    void SecureSession::setUid(uint32_t uid) {
        mUid = uid;
    }

    uint32_t SecureSession::getUid() const {
        return mUid;
    }

    void SecureSession::readHeader() {
        auto self = shared_from_this();
        mBuffer.resize(net::protocol::kHeaderSize);

        boost::asio::async_read(
            mStream,
            boost::asio::buffer(mBuffer),
            [this, self](auto ec, std::size_t) {
                if (ec) {
                    if (mErrorCallback) mErrorCallback(ec.message(), self);
                    return close();
                }

                uint32_t lenNet;
                std::memcpy(
                    &lenNet,
                    &mBuffer[net::protocol::kTypeFieldSize],
                    net::protocol::kLengthFieldSize
                );

                readBody(ntohl(lenNet));
            }
        );
    }

    void SecureSession::readBody(std::size_t payloadSize) {
        auto self = shared_from_this();
        mBuffer.resize(net::protocol::kHeaderSize + payloadSize);

        boost::asio::async_read(
            mStream,
            boost::asio::buffer(
                mBuffer.data() + net::protocol::kHeaderSize,
                payloadSize
            ),
            [this, self](auto ec, std::size_t) {
                if (ec) {
                    if (mErrorCallback) mErrorCallback(ec.message(), self);
                    return close();
                }

                MessageType type = static_cast<MessageType>(mBuffer[0]);
                std::vector<uint8_t> payload(
                    mBuffer.begin() + net::protocol::kHeaderSize,
                    mBuffer.end()
                );

                try {
                    if (mMessageCallback) {
                        Message msg = Message::decode(type, payload);
                        mMessageCallback(msg, self);
                    }
                } catch (const std::exception& e) {
                    if (mErrorCallback) mErrorCallback(e.what(), self);
                    return close();
                }

                readHeader();
            }
        );
    }

    void SecureSession::write(const Message& message) {
        auto self = shared_from_this();
        auto bytes = std::make_shared<std::vector<uint8_t>>(message.encode());

        boost::asio::post(
            mStream.get_executor(),
            [this, self, bytes]() {
                boost::asio::async_write(
                    mStream,
                    boost::asio::buffer(*bytes),
                    [this, self, bytes](auto ec, std::size_t) {
                        if (ec) {
                            if (mErrorCallback) mErrorCallback(ec.message(), self);
                            close();
                        }
                    }
                );
            }
        );
    }

} // namespace net::server::sessions
