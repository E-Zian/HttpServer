#pragma once
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "Interface/IDispatcher.h"
#include "Router.h"
#include "Connection.h"
#include <asio.hpp>
#include <vector>

class HttpServer {
public:
	using tcp = asio::ip::tcp;

	HttpServer(asio::io_context& io,int port,const IDispatcher& dispatcher);

	~HttpServer();

	asio::awaitable<void> serverListen();


private:
	asio::io_context& io_;
	tcp::acceptor acceptor_;
	// std::vector <std::weak_ptr<Connection>> connectionList_;
	static int totalConnections_;
	const IDispatcher& dispatcher_;
	int port_;

};


#endif

