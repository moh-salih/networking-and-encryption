// app/client/common/ClientAppContext.cpp
#include "app/net/client/common/ClientAppContext.h"
#include <iostream>

using net::client::ClientFactory;
using net::client::Client;
using net::protocol::Message;
using json = net::protocol::json;

void ClientAppContext::connect(const net::client::ClientConfig& cfg, const std::string& username) {
    disconnect();

    mLoginName = username;
    mClient = ClientFactory::create(mIo, cfg, Client::RunMode::Manual);

    wireClient();
    
    if (!mClient->connect(cfg.host, cfg.port)) {
        if (events.onError)
            events.onError("Client already running");
    }

}

void ClientAppContext::disconnect() {
    if (mClient) {
        mClient->close();
        mClient.reset();
    }

    mUsers.clear();
    mMessages.clear();
    mMyUid = 0;
    mMyName.clear();
}

void ClientAppContext::poll() {
    if (mClient)
        mClient->poll();
}

// ---------------- Chat actions ----------------

void ClientAppContext::sendPublic(const std::string& text) {
    if (!mClient || text.empty()) return;

    mMessages.push_back({mMyUid, mMyName, text, false});
    if (events.onStateUpdated) events.onStateUpdated();

    mClient->requestAsync("send_public", {{"text", text}}, [](json) {});
}

void ClientAppContext::sendPrivate(uint32_t toUid, const std::string& text) {
    if (!mClient || text.empty()) return;

    mMessages.push_back({mMyUid, mMyName, text, true});
    if (events.onStateUpdated) events.onStateUpdated();

    mClient->requestAsync(
        "send_private",
        {{"to_uid", toUid}, {"text", text}},
        [](json) {}
    );
}

// ---------------- Accessors ----------------

uint32_t ClientAppContext::myUid() const { return mMyUid; }
const std::string& ClientAppContext::myName() const { return mMyName; }

const std::unordered_map<uint32_t, User>& ClientAppContext::users() const {
    return mUsers;
}

const std::vector<ChatMessage>& ClientAppContext::messages() const {
    return mMessages;
}

// ---------------- Internal wiring ----------------

void ClientAppContext::wireClient() {
    mClient->onConnect([this] {
        if (events.onConnected) events.onConnected();

        mClient->requestAsync(
            "login",
            {{"name", mLoginName}},
            [this](const json& res) { handleLoginResponse(res); }
        );
    });

    mClient->onPush([this](const json& push) {
        handlePush(push);
    });

    mClient->onError([this](const std::string& msg) {
        if (events.onError) events.onError(msg);
    });

    mClient->onDisconnect([this] {
        if (events.onDisconnected) events.onDisconnected();
    });
}

void ClientAppContext::handleLoginResponse(const json& response) {
    if (response.contains("message")) {
        if (events.onError)
            events.onError("Login failed: " +
                           response.value("message", "Unknown"));
        disconnect();
        return;
    }

    mMyUid = response["uid"];
    mMyName = response["name"];

    mClient->requestAsync("client_list", {}, [this](const json& list) {
        if (list.contains("clients")) {
            for (const auto& c : list["clients"]) {
                mUsers[c["uid"]] = {c["uid"], c["name"]};
            }
        }

        if (events.onLoginSuccess) events.onLoginSuccess();
        if (events.onStateUpdated) events.onStateUpdated();
    });
}

void ClientAppContext::handlePush(const json& p) {
    std::string evt = p.value("event", "");

    if (evt == "user_joined") {
        mUsers[p["uid"]] = {p["uid"], p["name"]};
    }
    else if (evt == "user_left") {
        mUsers.erase(p["uid"]);
    }
    else if (evt == "public_message") {
        uint32_t fromUid = p.value("from_uid", 0);
        if (fromUid != mMyUid) {
            mMessages.push_back({
                fromUid,
                p["from_name"],
                p["text"],
                false
            });
        }
    }
    else if (evt == "private_message") {
         uint32_t fromUid = p.value("from_uid", 0);
        if (fromUid != mMyUid) {
            mMessages.push_back({
                fromUid,
                p["from_name"],
                p["text"],
                true
            });

            auto it = mUsers.find(fromUid);
            if (it != mUsers.end())
                it->second.unreadCount++;
        }
    }

    if (events.onStateUpdated)
        events.onStateUpdated();
}
