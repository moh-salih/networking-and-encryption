#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <memory>
#include <vector>
#include <atomic>

#include "net/core/ISession.h"
#include "net/protocol/Message.h"

namespace net::server::sessions {

class SecureSession : public net::core::ISession {
public:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using SslStream = boost::asio::ssl::stream<TcpSocket>;

    SecureSession(TcpSocket&& socket, boost::asio::ssl::context& sslCtx);

    // ISession
    void start() override;
    void onStart(const SessionCallback& handler) override;

    void close() override;
    void onClose(const SessionCallback& handler) override;

    void send(const net::protocol::Message& message) override;
    void onMessage(const MessageCallback& handler) override;

    void onError(const ErrorCallback& handler) override;

    void setUid(uint32_t uid) override;
    uint32_t getUid() const override;

private:
    void doHandshake();
    void readHeader();
    void readBody(std::size_t payloadSize);
    void write(const net::protocol::Message& message);

private:
    uint32_t mUid{};
    SslStream mStream;
    std::vector<uint8_t> mBuffer;
    std::atomic<bool> mIsClosed{false};

    // Callbacks
    SessionCallback mStartSessionCallback;
    SessionCallback mCloseSessionCallback;
    MessageCallback mMessageCallback;
    ErrorCallback mErrorCallback;
};

} // namespace net::server::sessions
