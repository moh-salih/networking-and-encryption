#include <iostream>

#include "net/server/Router.h"
#include "net/protocol/MessageType.h"
#include "net/protocol/Message.h"

using net::protocol::json;
using net::protocol::Message;

namespace net::server {

    Router::Router(){
        // Default handler
        mFallbackHandler = [](const Message& request, int code, const std::string& message) {
            uint32_t id = request.j.value("id", 0);
            return Message::makeError(id, code, message);
        };
    }
    
    void Router::add(const std::string& method, Handler handler) {
        std::lock_guard<std::mutex> lock(mMutex);
        mHandlers[method] = std::move(handler);
    }

    
    net::protocol::Message Router::handle(const net::protocol::Message& request, uint32_t uid){
        uint32_t id = request.j.value("id", 0);
        
        if (request.type != net::protocol::MessageType::Request) {
            return mFallbackHandler(request, -32600, "Invalid Request: Not a request");
        }

        std::string method = request.j.value("method", "");
        if (method.empty()) {
            return mFallbackHandler(request, -32600, "Invalid Request: No method");
        }

        json params = request.j.value("params", json::object());
        Handler handler;

        {
            std::lock_guard<std::mutex> lock(mMutex);
            auto it = mHandlers.find(method);
            if (it == mHandlers.end()) {
                // 2. Use the fallback for "Method not found"
                return mFallbackHandler(request, -32601, "Method not found: " + method);
            }
            handler = it->second;
        }

        try {
            json result = handler(params, uid);
            return net::protocol::Message::makeResponse(id, result);
        } 
        catch (const json::exception& e) {
            // 3. Use the fallback for try/catch errors
            return mFallbackHandler(request, -32001, "Handler JSON error: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            // 4. Use the fallback for all other errors
            return mFallbackHandler(request, -32000, "Server error: " + std::string(e.what()));
        }
    }

    void Router::setFallback(const ErrorHandler& handler) {
        std::lock_guard<std::mutex> lock(mMutex);
        mFallbackHandler = handler;
    }

    bool Router::exists(const std::string& method) const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mHandlers.find(method) != mHandlers.end();
    }

} // namespace net::server
