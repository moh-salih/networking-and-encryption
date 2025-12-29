#include <iostream>
#include <string>

#include "app/net/server/common/ServerAppContext.h"
#include "app/net/server/common/ServerArgs.h"

int main(int argc, char** argv) {
    try {
        auto args = app::server::parseServerArgs(argc, argv, "server");

        if (!args.config.isValid()) {
            std::cerr << "Invalid server configuration\n";
            return 1;
        }

        app::server::ServerAppContext ctx;

        if (!ctx.start(args.config)) {
            std::cerr << "Failed to start server\n";
            return 1;
        }

        std::cout << "Server running on port "
                  << args.config.port << "\n";
        std::cout << "Type 'exit' to stop\n\n";

        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "exit" || line == "quit" || line == "q")
                break;
        }

        ctx.stopAll();
    }
    catch (...) {
        return 1;
    }

    return 0;
}
