#include "app/net/client/common/ClientArgs.h"

#include <argparse/argparse.hpp>
#include <iostream>

namespace app::client {

ClientArgs parseClientArgs(int argc, char** argv, const char* appName) {
    argparse::ArgumentParser program(appName);

    ClientArgs args;

    program.add_argument("--host")
        .help("Server host")
        .default_value(args.host)
        .store_into(args.host);

    program.add_argument("--port")
        .help("Server port")
        .scan<'u', uint16_t>()
        .default_value(args.port)
        .store_into(args.port);

    program.add_argument("--secure")
        .help("Enable TLS")
        .default_value(false)
        .implicit_value(true)
        .store_into(args.secure);

    program.add_argument("--name")
        .help("Username")
        .default_value(args.name)
        .store_into(args.name);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n\n";
        std::cerr << program << "\n";
        throw;
    }

    return args;
}

} // namespace app::client
