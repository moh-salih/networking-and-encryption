#pragma once

// net/transport/PlainTransport.h
#pragma once

#include "net/core/ITransport.h"
#include <boost/asio.hpp>

namespace net::client::transport {

class PlainTransport : public ITransport {
public:
    using Socket = boost::asio::ip::tcp::socket;
    using Resolver = boost::asio::ip::tcp::resolver;

    explicit PlainTransport(boost::asio::io_context& io);

    // ITransport
    void connect(const std::string& host, uint16_t port, ErrorCallback cb) override;
    void asyncRead(ReadBuffer buffer, ReadCallback cb) override;
    void asyncWrite(const WriteBuffer& buffer, WriteCallback cb) override;
    void close() override;

private:
    boost::asio::io_context& mIo;
    Socket   mSocket;
    Resolver mResolver;
};

} // namespace net::client::transport
