#pragma once
#include <boost/asio/buffer.hpp>
#include <boost/system/error_code.hpp>
#include <functional>

class ITransport {
public:
    using ReadBuffer  = boost::asio::mutable_buffer;
    using WriteBuffer = boost::asio::const_buffer;

    using ReadCallback = std::function<void(boost::system::error_code, std::size_t)>;
    using WriteCallback = std::function<void(boost::system::error_code, std::size_t)>;
    using ErrorCallback = std::function<void(const boost::system::error_code&)>;

    virtual ~ITransport() = default;

    virtual void connect(const std::string& host, uint16_t port, ErrorCallback cb) = 0;


    virtual void asyncRead(ReadBuffer buffer, ReadCallback cb) = 0;

    virtual void asyncWrite(const WriteBuffer& buffer, WriteCallback cb) = 0;

    virtual void close() = 0;
};
