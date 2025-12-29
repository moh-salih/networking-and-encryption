#include <iostream>
#include <string>
#include <atomic>

#include "app/net/client/common/ClientAppContext.h"
#include "app/net/client/common/ClientArgs.h"

using namespace std;

int main(int argc, char** argv) {
    try {
        // --- Parse CLI args ---
        auto args = app::client::parseClientArgs(argc, argv, "client-cli");

        ClientAppContext ctx;
        atomic<bool> running{true};
        atomic<bool> redraw{false};

        // --- Wire events ---
        ctx.events.onConnected = [] {
            cout << "[connected]\n";
        };

        ctx.events.onLoginSuccess = [&] {
            cout << "[login success]\n";
            redraw = true;
        };

        ctx.events.onDisconnected = [&] {
            cout << "[disconnected]\n";
            running = false;
        };

        ctx.events.onError = [&](const string& err) {
            cout << "[error] " << err << "\n";
        };

        ctx.events.onStateUpdated = [&] {
            redraw = true;
        };

        // --- Connect ---
        net::client::ClientConfig cfg;
        cfg.host = args.host;
        cfg.port = args.port;
        cfg.mode = args.secure
            ? net::client::ClientMode::Secure
            : net::client::ClientMode::Plain;

        ctx.connect(cfg, args.name);

        cout << "Type messages and press Enter\n";
        cout << "Commands:\n";
        cout << "  /q                 quit\n";
        cout << "  /w <uid> <msg>     private message\n\n";

        // --- Main loop (single thread) ---
        while (running) {
            // 1. Pump network
            ctx.poll();

            // 2. Redraw state if needed
            if (redraw.exchange(false)) {
                const auto& users = ctx.users();
                const auto& msgs  = ctx.messages();

                cout << "\n--- USERS ---\n";
                for (const auto& [uid, u] : users) {
                    cout << "  " << uid << ": " << u.name;
                    if (uid == ctx.myUid()) cout << " (you)";
                    cout << "\n";
                }

                cout << "\n--- MESSAGES ---\n";
                for (const auto& m : msgs) {
                    cout << (m.isPrivate ? "[PM] " : "")
                         << m.fromName << ": "
                         << m.text << "\n";
                }

                cout << "----------------\n";
            }

            // 3. Non-blocking stdin check
            if (cin.rdbuf()->in_avail() > 0) {
                string line;
                getline(cin, line);

                if (line == "/q") {
                    running = false;
                    break;
                }

                // Private message: /w <uid> <msg>
                if (line.starts_with("/w ")) {
                    auto rest = line.substr(3);
                    auto sp = rest.find(' ');
                    if (sp != string::npos) {
                        uint32_t uid = static_cast<uint32_t>(stoul(rest.substr(0, sp)));
                        string msg = rest.substr(sp + 1);
                        ctx.sendPrivate(uid, msg);
                    }
                    continue;
                }

                // Public message
                ctx.sendPublic(line);
            }

            // 4. Small sleep to avoid 100% CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        ctx.disconnect();
        cout << "bye\n";
    }
    catch (const std::exception& e) {
        cerr << "Client error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
