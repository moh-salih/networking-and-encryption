#include "net/client/transport/PlainTransport.h"

namespace net::client::transport {

    using boost::asio::ip::tcp;

    PlainTransport::PlainTransport(boost::asio::io_context& io)
        : mIo(io)
        , mSocket(io)
        , mResolver(io)
    {
    }

    void PlainTransport::connect(const std::string& host, uint16_t port, ErrorCallback cb) {
        mResolver.async_resolve(
            host,
            std::to_string(port),
            [this, cb = std::move(cb)](auto ec, tcp::resolver::results_type results) {
                if (ec) {
                    cb(ec);
                    return;
                }

                boost::asio::async_connect(
                    mSocket,
                    results,
                    [cb = std::move(cb)](auto ec, const tcp::endpoint&) {
                        cb(ec);
                    }
                );
            }
        );
    }

    void PlainTransport::asyncRead(ReadBuffer buffer, ReadCallback cb) {
        boost::asio::async_read(
            mSocket,
            buffer,
            std::move(cb)
        );
    }

    void PlainTransport::asyncWrite(const WriteBuffer& buffer, WriteCallback cb) {
        boost::asio::async_write(
            mSocket,
            buffer,
            std::move(cb)
        );
    }

    void PlainTransport::close() {
        boost::system::error_code ec;
        mSocket.shutdown(tcp::socket::shutdown_both, ec);
        mSocket.close(ec);
    }

} // namespace net::client::transport
