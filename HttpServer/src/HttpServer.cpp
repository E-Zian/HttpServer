#include "Helper.h"
#include "HttpServer.h"
#include "Connection.h"
#include <asio.hpp>
#include <iostream>

int HttpServer::totalConnections_{};

HttpServer::HttpServer(asio::io_context &io, const int port, const IDispatcher& dispatcher)
    : io_{io},
      acceptor_{io, tcp::endpoint(tcp::v4(), port)},
      dispatcher_{dispatcher},
      port_{port}
 {
    asio::co_spawn(io, serverListen(), asio::detached);

}

asio::awaitable<void> HttpServer::serverListen() {
    while (true) {
        try {
            Helper::displayMessage("Waiting for connection...");

            asio::ip::tcp::socket socket{co_await acceptor_.async_accept(asio::use_awaitable)};

            Connection::pointer connection{Connection::create( std::move(socket), ++HttpServer::totalConnections_,dispatcher_)};
            connectionList_.push_back(connection);

            Helper::displayMessage("Connection ID : {} Connected", HttpServer::totalConnections_);

            asio::co_spawn(io_, connection->startRead(), asio::detached);
        } catch (std::exception &ex) {
            Helper::displayError("Http Server error , {}", ex.what());
        }
    }
}


HttpServer::~HttpServer() {
    Helper::displayMessage("Server on port ({}) closed",port_);
    connectionList_.clear();
}
