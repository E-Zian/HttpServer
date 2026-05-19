#include "HttpServer.h"
#include "Connection.h"
#include <asio.hpp>
#include <iostream>

int HttpServer::totalConnections{};

HttpServer::HttpServer(asio::io_context& io)
	:io{ io }, 
	acceptor_{ io,tcp::endpoint(tcp::v4(),6767) } 
{
	ConnectionList.reserve(10);

	asio::co_spawn(io, serverListen(), asio::detached);
}

asio::awaitable<void> HttpServer::serverListen() {
	while (true) {
		try {
			std::cout << "Waiting for connection \n";
			Connection::pointer connection{ Connection::create(io,co_await acceptor_.async_accept(asio::use_awaitable), ++HttpServer::totalConnections) };
			ConnectionList.push_back(connection);
			std::cout << "Connection ID : "<< HttpServer::totalConnections<<" Connected \n\n";
			asio::co_spawn(io, connection->startRead(), asio::detached);
			
		}
		catch (std::exception& ex) {
			std::cout << "Server Error : " << ex.what() << "\n";
		}

	}

}



HttpServer::~HttpServer() {
	std::cout << "~HttpServer \n";
	for (auto& connection : ConnectionList) {
		asio::co_spawn(io,connection.lock()->shutdown(),asio::detached) ;
	}
	ConnectionList.clear();
}
