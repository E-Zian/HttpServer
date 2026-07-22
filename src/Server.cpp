#include "Helper.h"
#include "Server.h"
#include "Connection.h"
#include <asio.hpp>
#include <iostream>

template <typename Stream>
Server<Stream>::Server(asio::io_context &io, const int port, const IDispatcher& dispatcher, RateLimiter& rateLimiter, asio::ssl::context* sslContext)
    : io_{io},
      acceptor_{io, tcp::endpoint(tcp::v4(), port)},
      totalConnections_{},
      dispatcher_{dispatcher},
      port_{port},
      rateLimiter_{ rateLimiter },
      sslContext_{sslContext}
{
    asio::co_spawn(io, serverListen(), asio::detached);

}

template <typename Stream>
Server<Stream>::~Server() {
    log("Server on port ({}) closed",port_);
}

template <typename Stream>
asio::awaitable<void> Server<Stream>::serverListen() {
    log("Initiated server listen on port ({})",port_);

    while (true) {
        try {
            log("Waiting for connection...");

            asio::ip::tcp::socket socket{co_await acceptor_.async_accept(asio::use_awaitable)};
            socket.set_option(asio::ip::tcp::no_delay(true));

            const typename Connection<Stream>::pointer connection{Connection<Stream>::create( std::move(socket), ++totalConnections_,dispatcher_,rateLimiter_,sslContext_)};

            log("Connection ID : {} Connected", totalConnections_.load());

            asio::co_spawn(io_, connection->handleRequest(), asio::detached);
        } catch (std::exception &ex) {
            logError("Http Server error , {}", ex.what());
        }
    }
}

template class Server<asio::ip::tcp::socket>;
template class Server<asio::ssl::stream<asio::ip::tcp::socket>>;