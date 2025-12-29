#pragma once

#include <memory>

#include "net/server/ServerController.h"
#include "net/server/Router.h"
#include "net/server/SessionManager.h"
#include "net/server/ServerConfig.h"

namespace app::server {

class ServerAppContext {
public:
    ServerAppContext();

    bool start(const net::server::ServerConfig& cfg);
    void stop(uint16_t port);
    void stopAll();

    std::shared_ptr<net::server::SessionManager> sessions() const;
    std::shared_ptr<net::server::Router> router() const;

private:
    void setupRoutes();

private:
    boost::asio::io_context mIo;
    std::shared_ptr<net::server::Router> mRouter;
    std::shared_ptr<net::server::SessionManager> mSessions;
    std::unique_ptr<net::server::ServerController> mController;
};

} // namespace app::server
