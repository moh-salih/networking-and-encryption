#include "net/server/Server.h"
#include <iostream>

using boost::asio::ip::tcp;
using net::protocol::json;

namespace net::server {

Server::Server(boost::asio::io_context& io, unsigned short port): mIo(io),mAcceptor(io, tcp::endpoint(tcp::v4(), port)) {

}

void Server::start() {
    if(mStartCallback) mStartCallback(mAcceptor.local_endpoint().port());
    accept();
}

void Server::stop() {
    boost::system::error_code ec;
    mAcceptor.close(ec);
    if(mStopCallback) mStopCallback(ec.message());
}

void Server::accept() {
    mAcceptor.async_accept([this](auto ec, tcp::socket sock) {
        if(!ec){
            mConnectCallback(std::move(sock));
            accept(); // continue accepting new clients
        }else if(ec != boost::asio::error::operation_aborted){
            mErrorCallback(ec.message());
        }            
    });
}

} // namespace net::server
