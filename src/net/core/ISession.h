#pragma once

#include <memory>
#include <functional>

#include "net/protocol/Message.h"

namespace net::core {
    class ISession;
    class ISession : public std::enable_shared_from_this<ISession> {
    public:
        // Callback signatures
        using SessionCallback = std::function<void(std::shared_ptr<ISession>)>;
        using MessageCallback = std::function<void(const net::protocol::Message&, std::shared_ptr<ISession>)>;
        using ErrorCallback = std::function<void(const std::string&, std::shared_ptr<ISession>)>;

        virtual ~ISession() = default;
        
        virtual void start() = 0;
        virtual void onStart(const SessionCallback& handler) = 0;

        virtual void close() = 0;
        virtual void onClose(const SessionCallback& handler) = 0;
        
        virtual void send(const net::protocol::Message& message) = 0;
        virtual void onMessage(const MessageCallback& handler) = 0;

        virtual void onError(const ErrorCallback& handler) = 0;
        
        virtual void setUid(uint32_t uid) = 0;
        virtual uint32_t getUid() const = 0;
    };
}
