#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <cstdint>
#include <chrono>
#include <mutex>
#include <atomic>

#include "net/core/ISession.h"
#include "net/protocol/Message.h"

namespace net::server {

    class SessionManager {
    public:
        SessionManager() = default;
        SessionManager(const SessionManager&) = delete;
        SessionManager(SessionManager&&) = delete;
        SessionManager& operator=(const SessionManager&) = delete; 
        SessionManager& operator=(SessionManager&&) = delete;


        uint32_t add(const std::shared_ptr<net::core::ISession>& session);

        void remove(uint32_t uid);

        std::shared_ptr<net::core::ISession> get(uint32_t uid);


        void broadcast(const net::protocol::Message& message);
        void sendTo(const std::vector<uint32_t>& uids, const net::protocol::Message& message);

        void setName(uint32_t uid, const std::string& name);
        std::string getName(uint32_t uid) const;


        size_t getCount() const;
        std::vector<uint32_t> listIds() const;

    private:
        struct SessionInfo {
            std::shared_ptr<net::core::ISession> session;
            std::chrono::system_clock::time_point startTime;
            std::string username {"guest"};
        };

        mutable std::mutex mMutex; // marked mutable to allow locking it from const methods
        std::atomic<uint32_t> mNextUid{1};
        std::unordered_map<uint32_t, SessionInfo> mSessions;
    };

} // namespace net::server
