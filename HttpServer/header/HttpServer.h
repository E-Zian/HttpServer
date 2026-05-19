#pragma once
#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <asio.hpp>

class HttpServer {
public:
	using tcp = asio::ip::tcp;
	HttpServer(asio::io_context& io);
	static int totalConnections;

	asio::awaitable<void> serverListen();
private:
	asio::io_context& io;
	tcp::acceptor acceptor_;
};


#endif

