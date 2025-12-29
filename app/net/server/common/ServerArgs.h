#pragma once

#include <cstdint>
#include <string>

#include "net/server/ServerConfig.h"

namespace app::server {

    struct ServerArgs {
        net::server::ServerConfig config;
    };

    ServerArgs parseServerArgs(int argc, char** argv, const char* appName);

} // namespace app::server
