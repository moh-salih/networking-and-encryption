#pragma once

#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include "net/core/IServer.h"
#include "net/server/Router.h"
#include "net/server/SessionManager.h"

namespace net::server {

class Server : public net::core::IServer {
public:
    using ConnectCallback = std::function<void(boost::asio::ip::tcp::socket)>;
    using ErrorCallback = std::function<void(const std::string&)>;
    using StartCallback = std::function<void(unsigned int)>;
    using StopCallback = std::function<void(const std::string&)>;

    Server(boost::asio::io_context& io, unsigned short port);

    void start() override;
    void stop() override;

    inline void onConnect(const ConnectCallback& callback) {
        mConnectCallback = callback;
    }

    inline void onError(const ErrorCallback& callback) {
        mErrorCallback = callback;
    }

    inline void onStart(const StartCallback& callback) {
        mStartCallback = callback;
    }

    inline void onStop(const StopCallback& callback) {
        mStopCallback = callback;
    }

private:
    void accept();

private:
    boost::asio::io_context& mIo;
    boost::asio::ip::tcp::acceptor mAcceptor;


    // Callbacks
    ConnectCallback mConnectCallback; 
    ErrorCallback mErrorCallback; 
    StartCallback mStartCallback;
    StopCallback mStopCallback; 
};

} // namespace net::server
