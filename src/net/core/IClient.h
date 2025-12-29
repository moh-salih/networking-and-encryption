#pragma once

#include <string>
#include <functional>
#include "net/protocol/Json.h" 

namespace net::core {
    
    class IClient {
    public:
        using json = net::protocol::json;
        
        //  Callback Signatures 
        using ResponseCallback = std::function<void(const json& result)>;
        using PushHandler = std::function<void(const json& pushBody)>;
        using VoidCallback = std::function<void()>;
        using ErrorCallback = std::function<void(const std::string& errorMsg)>;

        virtual ~IClient() = default;

        // Connection & Lifecycle
        
        // connect to the server with given host and port
        virtual bool connect(const std::string& host, uint16_t port) = 0;
        
        // Closes the connection.
        virtual void close() = 0;
        
        // Checks if the client is connected.
        virtual bool isRunning() const = 0;
        
        // Required if client is in Manual run-mode.
        virtual void poll() = 0;

        // Asynchronous Requests (RPC)

        // Send an async request to `method` and get the result later in the callback passed
        virtual void requestAsync(const std::string& method, const json& params, ResponseCallback cb) = 0;

        // Send a synchronous(blocking) request.
        virtual json request(const std::string& method, const json& params = json::object()) = 0;

        // Event Handlers (Subscriptions)

        // Fires when connection is successfully established.
        virtual void onConnect(VoidCallback handler) = 0;

        // Fires when the connection is lost or closed.
        virtual void onDisconnect(VoidCallback handler) = 0;
        
        // Fires on an I/O or protocol error.
        virtual void onError(ErrorCallback handler) = 0;
        
        // Main handler for all server-push messages. 
        virtual void onPush(PushHandler handler) = 0;
    };

} // namespace net::core