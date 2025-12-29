#include "app/net/server/common/ServerAppContext.h"
#include "net/protocol/Message.h"

using json = net::protocol::json;
using Message = net::protocol::Message;

namespace app::server {

ServerAppContext::ServerAppContext() {
    mRouter   = std::make_shared<net::server::Router>();
    mSessions = std::make_shared<net::server::SessionManager>();
    mController = std::make_unique<net::server::ServerController>(mRouter, mSessions);

    setupRoutes();
}

bool ServerAppContext::start(const net::server::ServerConfig& cfg) {
    return mController->start(cfg);
}

void ServerAppContext::stop(uint16_t port) {
    mController->stop(port);
}

void ServerAppContext::stopAll() {
    mController->stopAll();
}

std::shared_ptr<net::server::SessionManager> ServerAppContext::sessions() const {
    return mSessions;
}

std::shared_ptr<net::server::Router> ServerAppContext::router() const {
    return mRouter;
}

void ServerAppContext::setupRoutes() {
    mRouter->add("login", [this](const json& p, uint32_t uid) -> json {
        std::string name = p.value("name", "guest");
        mSessions->setName(uid, name);
        mSessions->broadcast(Message::makePush({{"event", "user_joined"}, {"uid", uid}, {"name", name}}));
        return {{"uid", uid}, {"name", name}, {"status", "success"}};
    });
    
    mRouter->add("client_list", [this](const json&, uint32_t) -> json {
        json arr = json::array();
        for (auto id : mSessions->listIds()) arr.push_back({{"uid", id}, {"name", mSessions->getName(id)}});
        return {{"clients", arr}};
    });

    mRouter->add("send_public", [this](const json& p, uint32_t uid) -> json {
        std::string text = p.value("text", "");
        std::string name = mSessions->getName(uid);
        mSessions->broadcast(Message::makePush({{"event", "public_message"}, {"from_uid", uid}, {"from_name", name}, {"text", text}}));
        return {{"delivered", true}};
    });

    mRouter->add("send_private", [this](const json& p, uint32_t uid) -> json {
        uint32_t to = p.value("to_uid", 0);
        std::string text = p.value("text", "");
        std::string from = mSessions->getName(uid);
        mSessions->sendTo({to}, Message::makePush({{"event", "private_message"}, {"from_uid", uid}, {"from_name", from}, {"text", text}}));
        return {{"delivered", true}};
    });
    
    mRouter->add("ping", [](const json&, uint32_t) -> json { return {{"msg", "pong"}}; });
}

} // namespace app::server
