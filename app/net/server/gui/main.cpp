#include <iostream>

#include "app/net/common/CoreApplication.h"
#include "app/net/server/gui/ServerDashboard.h"
#include "app/net/server/common/ServerArgs.h"


#include "app/net/resources/server_icon.h"

int main(int argc, char** argv) {
    try {
        auto args = app::server::parseServerArgs(argc, argv, "server-gui");

        auto dashboard = std::make_shared<ServerDashboard>(args.config);
        CoreApplication app(dashboard);

        app.setTitle("Chat Server Dashboard");
        app.setWindowIcon(server_png, server_png_len);

        app.run();
    }
    catch (const std::exception& e) {
        std::cout << "Server: " << e.what() << std::endl;

        return 1;
    }

    return 0;
}
