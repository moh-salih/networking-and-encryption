#pragma once

#include <unordered_map>
#include <memory>
#include <thread>
#include <cstdint>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "net/server/Server.h"
#include "net/server/Router.h"
#include "net/server/SessionManager.h"
#include "net/server/sessions/PlainSession.h"
#include "net/server/sessions/SecureSession.h"
#include "net/server/ServerConfig.h"

namespace net::server {


    class ServerController {
    public:
        ServerController(
            std::shared_ptr<Router> router,
            std::shared_ptr<SessionManager> sessions
        );

        ~ServerController();

        bool start(const ServerConfig& cfg);
        void stop(uint16_t port);
        void stopAll();

        bool isRunning(uint16_t port) const;

        void loadTls(const std::string& cert, const std::string& key);

    private:
        struct Instance {
            ServerConfig cfg;
            std::unique_ptr<boost::asio::io_context> io;
            std::unique_ptr<boost::asio::executor_work_guard<
                boost::asio::io_context::executor_type>> work;
            std::unique_ptr<Server> server;
            std::thread thread;
        };

        
    private:
        std::unordered_map<uint16_t, Instance> mServers;

        std::shared_ptr<Router> mRouter;
        std::shared_ptr<SessionManager> mSessions;

        boost::asio::ssl::context mSslContext;
    };

} // namespace net::server
