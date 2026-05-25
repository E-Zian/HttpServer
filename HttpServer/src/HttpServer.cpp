#include "HttpServer.h"
#include "Connection.h"
#include <asio.hpp>
#include <iostream>

int HttpServer::totalConnections_{};

HttpServer::HttpServer(asio::io_context& io)
	:io_{ io },
	acceptor_{ io,tcp::endpoint(tcp::v4(),6767) } 
{
	connectionList_.reserve(10);

	asio::co_spawn(io, serverListen(), asio::detached);
}

asio::awaitable<void> HttpServer::serverListen() {
	while (true) {
		try {
			std::cout << "Waiting for connection \n";
			Connection::pointer connection{ Connection::create(io_,co_await acceptor_.async_accept(asio::use_awaitable), ++HttpServer::totalConnections_) };
			connectionList_.push_back(connection);
			std::cout << "Connection ID : "<< HttpServer::totalConnections_<<" Connected \n\n";
			asio::co_spawn(io_, connection->startRead(), asio::detached);
			
		}
		catch (std::exception& ex) {
			std::cout << "Server Error : " << ex.what() << "\n";
		}

	}

}


HttpServer::~HttpServer() {
	std::cout << "~HttpServer \n";
	for (auto& connection : connectionList_) {
		asio::co_spawn(io_,connection.lock()->shutdown(),asio::detached) ;
	}
	connectionList_.clear();
}
