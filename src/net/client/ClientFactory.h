#pragma once

#include <memory>
#include <string>
#include <cstdint>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "net/core/IClient.h"
#include "net/client/Client.h"

namespace net::client {

    enum class ClientMode {
        Plain,
        Secure
    };

    struct ClientConfig {
        ClientMode mode = ClientMode::Plain;
        std::string host;
        uint16_t port = 0;

        // Optional (future-proofing)
        bool verifyPeer = true;
    };

    class ClientFactory {
    public:
        static std::unique_ptr<net::core::IClient>
        create(boost::asio::io_context& io, const ClientConfig& cfg, net::client::Client::RunMode runMode = net::client::Client::RunMode::Threaded
        );
    };

} // namespace net::client
