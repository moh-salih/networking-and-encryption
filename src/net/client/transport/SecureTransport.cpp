#include "net/client/transport/SecureTransport.h"
#include <openssl/ssl.h>


namespace net::client::transport {
    SecureTransport::SecureTransport(
        boost::asio::io_context& io,
        std::shared_ptr<boost::asio::ssl::context> ctx
    )
        : mCtx(std::move(ctx))
        , mStream(io, *mCtx)
        , mResolver(io)
    {
    }


    void SecureTransport::connect(const std::string& host, uint16_t port, ErrorCallback cb) {
        mResolver.async_resolve(
            host,
            std::to_string(port),
            [this, host, cb = std::move(cb)](
                const boost::system::error_code& ec,
                boost::asio::ip::tcp::resolver::results_type results
            ) {
                if (ec) return cb(ec);

                boost::asio::async_connect(
                    mStream.next_layer(),
                    results,
                    [this, host, cb](const boost::system::error_code& ec, const auto&) {
                        if (ec) return cb(ec);

                        // ✅ SNI (you did this correctly)
                        SSL_set_tlsext_host_name(
                            mStream.native_handle(),
                            host.c_str()
                        );

                        mStream.async_handshake(
                            boost::asio::ssl::stream_base::client,
                            cb
                        );
                    }
                );
            }
        );
    }

    void SecureTransport::asyncRead(ReadBuffer buffer, ReadCallback cb) {
        boost::asio::async_read(mStream, buffer, std::move(cb));
    }

    void SecureTransport::asyncWrite(const WriteBuffer& buffer, WriteCallback cb) {
        boost::asio::async_write(mStream, buffer, std::move(cb));
    }

    void SecureTransport::close() {
        boost::system::error_code ec;
        mStream.shutdown(ec);
        mStream.next_layer().close(ec);
    }

} // namespace net::client::transport