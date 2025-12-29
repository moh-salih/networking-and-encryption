#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include "net/protocol/Json.h"
#include "net/protocol/Message.h"
#include <mutex>

namespace net::server {

class Router {
public:
    using Handler = std::function<net::protocol::json(const net::protocol::json& params, uint32_t uid)>;
    using ErrorHandler = std::function<net::protocol::Message(const net::protocol::Message& request, int code, const std::string& message)>;

    Router();

    void add(const std::string& method, Handler handler);

    net::protocol::Message handle(const net::protocol::Message& request, uint32_t uid);

    void setFallback(const ErrorHandler& handler);

    bool exists(const std::string& method) const;

private:
    std::unordered_map<std::string, Handler> mHandlers;
    mutable std::mutex mMutex;
    ErrorHandler mFallbackHandler;
};

} // namespace net::server
