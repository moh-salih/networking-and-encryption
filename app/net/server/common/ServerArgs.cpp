#include "app/net/server/common/ServerArgs.h"

#include <argparse/argparse.hpp>
#include <iostream>

namespace app::server {

ServerArgs parseServerArgs(int argc, char** argv, const char* appName) {
    argparse::ArgumentParser program(appName);

    ServerArgs result;
    bool secure = false;

    program.add_argument("--port")
        .help("Server port")
        .scan<'u', uint16_t>()
        .default_value(uint16_t{12345})
        .store_into(result.config.port);

    program.add_argument("--secure")
        .help("Enable TLS")
        .default_value(false)
        .implicit_value(true)
        .store_into(secure);

    program.add_argument("--cert")
        .help("TLS certificate file")
        .default_value(std::string("server.crt"))
        .store_into(result.config.certFile);

    program.add_argument("--key")
        .help("TLS private key file")
        .default_value(std::string("server.key"))
        .store_into(result.config.keyFile);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n\n";
        std::cerr << program << "\n";
        throw;
    }

    result.config.mode = secure
        ? net::server::ServerMode::Secure
        : net::server::ServerMode::Plain;

    return result;
}

} // namespace app::server
