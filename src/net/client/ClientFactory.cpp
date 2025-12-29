#include "net/client/ClientFactory.h"

#include "net/client/Client.h"
#include "net/client/transport/PlainTransport.h"
#include "net/client/transport/SecureTransport.h"


#include <stdexcept>

namespace net::client {

    std::unique_ptr<net::core::IClient>
    ClientFactory::create(boost::asio::io_context& io, const ClientConfig& cfg, net::client::Client::RunMode runMode) {
        
        if (cfg.port == 0) {
            throw std::invalid_argument("ClientFactory: port must be non-zero");
        }

        std::unique_ptr<ITransport> transport;

        if (cfg.mode == ClientMode::Plain) {

            transport = std::make_unique<transport::PlainTransport>(io);

        }  else { // Secure
            auto ctx = std::make_shared<boost::asio::ssl::context>(
                boost::asio::ssl::context::tls_client
            );

            ctx->set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3);

            if (cfg.verifyPeer) {
                ctx->set_default_verify_paths();
                ctx->set_verify_mode(boost::asio::ssl::verify_peer);
            } else {
                ctx->set_verify_mode(boost::asio::ssl::verify_none);
            }

            transport = std::make_unique<transport::SecureTransport>(
                io,
                ctx
            );
        }


        return std::make_unique<net::client::Client>(io, std::move(transport), runMode);
    }

} // namespace net::client
