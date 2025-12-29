#pragma once

#include <string>
#include <cstdint>

namespace app::client {

    struct ClientArgs {
        std::string host = "127.0.0.1";
        uint16_t port    = 12345;
        bool secure      = false;
        std::string name = "guest";
    };

    ClientArgs parseClientArgs(int argc, char** argv, const char* appName);

} // namespace app::client
