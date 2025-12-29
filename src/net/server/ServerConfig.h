#pragma once
#include <cstdint>
#include <string>

namespace net::server {

    enum class ServerMode {
        Plain,
        Secure
    };

    struct ServerConfig {
        uint16_t port = 0;
        ServerMode mode = ServerMode::Plain;

        // Only used if mode == Secure
        std::string certFile;
        std::string keyFile;

        bool isValid() const;
    };

} // namespace net::server
