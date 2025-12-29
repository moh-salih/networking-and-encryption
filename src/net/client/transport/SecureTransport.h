#pragma once
#include <vector>
#include "net/core/ITransport.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


namespace net::client::transport {

    class SecureTransport  : public ITransport {
    public:
        using SocketType = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
        using ErrorCallback = std::function<void(const boost::system::error_code&)>;
        using EndpointsType = boost::asio::ip::tcp::resolver::results_type;
        
        SecureTransport(boost::asio::io_context& io, std::shared_ptr<boost::asio::ssl::context> ctx);
        
        // ITransport implementation
        void connect(const std::string& host, uint16_t port, ErrorCallback cb) override;
        void asyncRead(ReadBuffer buffer, ReadCallback cb) override;
        void asyncWrite(const WriteBuffer& buffer, WriteCallback cb) override;
        void close() override;

    private:
        SocketType mStream;
        boost::asio::ip::tcp::resolver mResolver;
        std::shared_ptr<boost::asio::ssl::context> mCtx;
    };

} // namespace net::client::transport