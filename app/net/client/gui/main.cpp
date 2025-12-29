#include "app/net/common/CoreApplication.h"
#include "app/net/client/gui/ChatApplication.h"
#include "app/net/client/common/ClientArgs.h"

#include <iostream>

#include "app/net/resources/client_icon.h"

int main(int argc, char * argv[]) {
    try {
        // 🔹 Parse CLI args (even for GUI)
        auto args = app::client::parseClientArgs(argc, argv, "client-gui");

        auto chatApp = std::make_shared<ChatApplication>();

        // 🔹 Pre-fill UI defaults from args
        chatApp->setInitialArgs(args);

        CoreApplication app(chatApp);
        app.setTitle("Modern Chat");
        app.setWindowIcon(client_png, client_png_len);
        app.run();
    }
    catch (const std::exception& e) {
        std::cout << "Client: " << e.what() << std::endl;
    }
    catch (...) {
        std::cout << "Client: unknown exception" << std::endl;
    }
    return 0;
}
