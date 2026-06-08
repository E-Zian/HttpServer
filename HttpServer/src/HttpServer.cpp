#include "Helper.h"
#include "HttpServer.h"
#include "Connection.h"
#include <asio.hpp>
#include <iostream>

int HttpServer::totalConnections_{};

HttpServer::HttpServer(asio::io_context &io,int port, Router &router)
    : io_{io},
      acceptor_{io, tcp::endpoint(tcp::v4(), port)},
      router_{router} {
    asio::co_spawn(io, serverListen(), asio::detached);

}

asio::awaitable<void> HttpServer::serverListen() {
    while (true) {
        try {
            Helper::displayMessage("Waiting for connection \n");

            asio::ip::tcp::socket socket{co_await acceptor_.async_accept(asio::use_awaitable)};

            Connection::pointer connection{Connection::create( std::move(socket), ++HttpServer::totalConnections_)};
            connectionList_.push_back(connection);

            Helper::displayMessage("Connection ID : {} Connected \n", HttpServer::totalConnections_);

            asio::co_spawn(io_, connection->startRead(), asio::detached);
        } catch (std::exception &ex) {
            Helper::displayError("{}\n", ex.what());
        }
    }
}


HttpServer::~HttpServer() {
    Helper::displayMessage("Server Closed");
    connectionList_.clear();
}
