#pragma once
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "RateLimiter.h"
#include "Interface/IDispatcher.h"
#include "Router.h"
#include "Connection.h"
#include <asio.hpp>

template <typename Stream>
class Server {
public:
	using tcp = asio::ip::tcp;

	Server(asio::io_context& io,int port,const IDispatcher& dispatcher, RateLimiter& rateLimiter, asio::ssl::context& sslContext);

	~Server();

	asio::awaitable<void> serverListen();


private:
	asio::io_context& io_;
	tcp::acceptor acceptor_;
	size_t totalConnections_;
	const IDispatcher& dispatcher_;
	int port_;
	RateLimiter& rateLimiter_;
	asio::ssl::context& sslContext_;

	template<typename... Args>
void log(fmt::format_string<Args...> fmt_string, Args &&... args) {
		Helper::displayMessageWithPort(port_,fmt_string,std::forward<Args>(args)...);
	}

	template<typename... Args>
void logError(fmt::format_string<Args...> fmt_string, Args &&... args) {
		Helper::displayErrorWithPort(port_,fmt_string,std::forward<Args>(args)...);
	}
};


#endif

