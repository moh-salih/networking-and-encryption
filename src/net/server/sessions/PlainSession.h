#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <atomic>

#include "net/core/ISession.h"
#include "net/protocol/Message.h"

namespace net::server::sessions {
    class PlainSession : public net::core::ISession {
    public:
        using TcpSocket = boost::asio::ip::tcp::socket;
        explicit PlainSession(TcpSocket&& socket);

        // ISession Interface implementation
        void start() override;
        void onStart(const SessionCallback& handler) override;

        void close() override;
        void onClose(const SessionCallback& handler) override;
        
        void send(const net::protocol::Message& message) override;
        void onMessage(const MessageCallback& handler) override;

        
        void onError(const ErrorCallback& handler) override;

        void setUid(uint32_t uid) override;
        uint32_t getUid() const override;

    protected:
        void read();
        void write(const net::protocol::Message& message);
    private:
        void readHeader();
        void readBody(size_t);

        void onHeaderRead(const boost::system::error_code& ec);
        void onBodyRead(const boost::system::error_code& ec, std::size_t payloadSize);

    private:
        uint32_t mUid{};
        TcpSocket mSocket;
        std::vector<uint8_t> mBuffer;
        std::atomic<bool> mIsClosed{false};
        
        // Event handlers
        SessionCallback mStartSessionCallback;
        SessionCallback mCloseSessionCallback;
        MessageCallback mMessageCallback;
        ErrorCallback mErrorCallback;
    };

} // namespace net::server::sessions