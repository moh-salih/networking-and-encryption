// app/client/common/ClientAppContext.h
#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

#include <boost/asio.hpp>

#include "app/net/client/common/models.h"
#include "net/protocol/Json.h"
#include "net/core/IClient.h"
#include "app/net/client/common/ClientArgs.h"
#include "net/client/ClientFactory.h"


class ClientAppContext {
public:
    using json = net::protocol::json;

    struct Events {
        std::function<void()> onConnected;
        std::function<void()> onLoginSuccess;
        std::function<void(const std::string&)> onError;
        std::function<void()> onDisconnected;
        std::function<void()> onStateUpdated; // generic UI refresh trigger
    };

    ClientAppContext() = default;

    // ---- Lifecycle ----
    void connect(const net::client::ClientConfig& cfg, const std::string& username);

    void disconnect();
    void poll();

    // ---- Chat actions ----
    void sendPublic(const std::string& text);
    void sendPrivate(uint32_t toUid, const std::string& text);

    // ---- Read-only access for UI ----
    uint32_t myUid() const;
    const std::string& myName() const;

    const std::unordered_map<uint32_t, User>& users() const;
    const std::vector<ChatMessage>& messages() const;

    Events events;

private:
    // ---- Internal handlers ----
    void wireClient();
    void handleLoginResponse(const json& response);
    void handlePush(const json& push);

private:
    boost::asio::io_context mIo;
    std::unique_ptr<net::core::IClient> mClient;

    // ---- Domain state ----
    uint32_t mMyUid{0};
    std::string mMyName;
    std::string mLoginName;

    std::unordered_map<uint32_t, User> mUsers;
    std::vector<ChatMessage> mMessages;
};
