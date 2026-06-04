#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#include "HttpTypes.h"
#include <asio.hpp>
#include <memory>
#include <unordered_map>

class Connection :public std::enable_shared_from_this<Connection> {
public:
	using tcp = asio::ip::tcp;
	using pointer = std::shared_ptr<Connection>;

	static pointer create(asio::io_context& io, tcp::socket&& connectionSocket,int connectionId);

	~Connection();

	asio::awaitable<void> startRead();

private:
	asio::io_context& io_;
	tcp::socket socket_;
	int connectionId_;
	std::vector<char> requestReceived_{};
	RequestObject request_;
	ParsedRequestObject parsedRequest_;

	Connection(asio::io_context& io, tcp::socket&& connectionSocket, int connectionId);

	asio::awaitable<void> writeResponse(std::string_view response);

	// std::string generateResponse();

	std::string generateDummyResponse();
};


#endif