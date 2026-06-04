#pragma once
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "Connection.h"
#include <asio.hpp>
#include <vector>

class HttpServer {
public:
	using tcp = asio::ip::tcp;

	explicit HttpServer(asio::io_context& io);

	~HttpServer();

	asio::awaitable<void> serverListen();


private:
	asio::io_context& io_;
	tcp::acceptor acceptor_;
	std::vector <std::weak_ptr<Connection>> connectionList_;
	static int totalConnections_;

};


#endif

