#include "net/server/SessionManager.h"
#include <iostream>

using net::protocol::json;

namespace net::server {
    
    uint32_t SessionManager::add(const std::shared_ptr<net::core::ISession>& session){
        uint32_t uid = mNextUid.fetch_add(1);

        SessionInfo info;
        info.session = session;
        info.startTime = std::chrono::system_clock::now();

        std::lock_guard<std::mutex> lock(mMutex);
        mSessions[uid] = std::move(info);

        return uid;
    }


    void SessionManager::remove(uint32_t uid){
        std::lock_guard<std::mutex> lock(mMutex);

        auto it = mSessions.find(uid);
        if(it == mSessions.end())  return;

        auto duration = std::chrono::system_clock::now() - it->second.startTime;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        mSessions.erase(it);
    }


   
    void SessionManager::broadcast(const net::protocol::Message& message){
        std::vector<std::shared_ptr<net::core::ISession>> allSessions;

        {
            std::lock_guard<std::mutex> lock(mMutex);
            allSessions.reserve(mSessions.size());

            for(auto &sessionInfo: mSessions){
                allSessions.push_back(sessionInfo.second.session);
            }
        }


        for(auto& session: allSessions) session->send(message);
    } 

    void SessionManager::sendTo(const std::vector<uint32_t>& uids, const net::protocol::Message& message) {
        std::vector<std::shared_ptr<net::core::ISession>> targets;

        {
            std::lock_guard<std::mutex> lock(mMutex);
            
            for (uint32_t uid : uids) {
                auto it = mSessions.find(uid);
                if (it != mSessions.end()) {
                    targets.push_back(it->second.session);
                }
            }
        } 

        for (auto& session : targets) {
            session->send(message);
        }
    }

    void SessionManager::setName(uint32_t uid, const std::string& name) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mSessions.find(uid);
        if(it != mSessions.end()){
            it->second.username = name;
        }

    }

    std::string SessionManager::getName(uint32_t uid) const {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mSessions.find(uid);
        if(it != mSessions.end()){
            return it->second.username;
        }

        return "Unknown";
    }

    std::shared_ptr<net::core::ISession> SessionManager::get(uint32_t uid) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mSessions.find(uid);
        if(it != mSessions.end()){
            return it->second.session;
        }

        return nullptr;
    }


    
    size_t SessionManager::getCount() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mSessions.size();
    }

    
    std::vector<uint32_t> SessionManager::listIds() const {
        std::vector<uint32_t> ids;
        {
            std::lock_guard<std::mutex> lock(mMutex);
            ids.reserve(mSessions.size());
            for(auto& sessionInfo: mSessions){
                ids.push_back(sessionInfo.first);
            }
        }

        return ids;
    }

} // namespace net::server
