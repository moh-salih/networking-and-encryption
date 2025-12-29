#include "net/server/ServerController.h"

#include <iostream>

namespace net::server {

ServerController::ServerController(
    std::shared_ptr<Router> router,
    std::shared_ptr<SessionManager> sessions
)
    : mRouter(std::move(router))
    , mSessions(std::move(sessions))
    , mSslContext(boost::asio::ssl::context::tls_server)
{
    // Minimal TLS setup (admin can configure later)
    mSslContext.set_options(
        boost::asio::ssl::context::default_workarounds |
        boost::asio::ssl::context::no_sslv2 |
        boost::asio::ssl::context::no_sslv3 |
        boost::asio::ssl::context::single_dh_use
    );

    // NOTE:
    // Certificates are NOT loaded here on purpose.
    // Console / GUI can configure them later.
}

void ServerController::loadTls(const std::string& cert, const std::string& key) {
    mSslContext.use_certificate_chain_file(cert);
    mSslContext.use_private_key_file(key, boost::asio::ssl::context::pem);
}


ServerController::~ServerController() {
    stopAll();
}

bool ServerController::start(const ServerConfig& cfg) {
    if (!cfg.isValid())    return false;

    if (mServers.contains(cfg.port))
        return false;

    if (cfg.mode == ServerMode::Secure) {
        loadTls(cfg.certFile, cfg.keyFile);
    }
    
    Instance inst;
    inst.cfg = cfg;
    inst.io = std::make_unique<boost::asio::io_context>();
    inst.work = std::make_unique<boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type>>(inst.io->get_executor());

    try {
        inst.server = std::make_unique<Server>(*inst.io, cfg.port);
    } catch (const std::exception& e) {
        std::cerr << "Failed to bind port " << cfg.port << ": " << e.what() << "\n";
        return false;
    }

    // ---- Connection Factory ----
    inst.server->onConnect([this, cfg](boost::asio::ip::tcp::socket socket) {
        std::shared_ptr<net::core::ISession> session;

        if (cfg.mode == ServerMode::Plain) {
            session = std::make_shared<sessions::PlainSession>(std::move(socket));
        } else {
            session = std::make_shared<sessions::SecureSession>(
                std::move(socket),
                mSslContext
            );
        }

        uint32_t uid = mSessions->add(session);
        session->setUid(uid);

        session->onClose([this, uid](auto) {
            mSessions->remove(uid);
            mSessions->broadcast(
                net::protocol::Message::makePush({
                    {"event", "user_left"},
                    {"uid", uid}
                })
            );
        });

        session->onMessage([this, uid](const net::protocol::Message& msg, auto s) {
            auto response = mRouter->handle(msg, uid);
            s->send(response);
        });

        session->start();
    });

    inst.server->start();
    inst.thread = std::thread([io = inst.io.get()] {
        io->run();
    });

    mServers.emplace(cfg.port, std::move(inst));
    return true;
}

void ServerController::stop(uint16_t port) {
    auto it = mServers.find(port);
    if (it == mServers.end())
        return;

    auto& inst = it->second;

    inst.work.reset();
    inst.server->stop();
    inst.io->stop();

    if (inst.thread.joinable())
        inst.thread.join();

    mServers.erase(it);
}

void ServerController::stopAll() {
    auto ports = std::vector<uint16_t>{};
    for (auto& [port, _] : mServers)
        ports.push_back(port);

    for (auto port : ports)
        stop(port);
}

bool ServerController::isRunning(uint16_t port) const {
    return mServers.contains(port);
}

} // namespace net::server
