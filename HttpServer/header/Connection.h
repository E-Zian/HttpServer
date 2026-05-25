#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#include "HttpTypes.h"
#include <memory>
#include <asio.hpp>

class Connection :public std::enable_shared_from_this<Connection> {
public:
	using tcp = asio::ip::tcp;
	using pointer = std::shared_ptr<Connection>;

	static pointer create(asio::io_context& io, tcp::socket&& connectionSocket,int connectionId);

	static std::string serverShutdownMessage_;

	~Connection();

	tcp::socket& getSocket() {
		return socket_;
	}

	asio::awaitable<void> startRead();
	asio::awaitable<void> shutdown();

private:
	tcp::socket socket_;
	int connectionId_;
	std::array<char, 128> receivingBuffer_{};
	std::vector<char> requestReceived_{};

	Connection(asio::io_context& io, tcp::socket&& connectionSocket, int connectionId);

	static std::optional<ParsedRequest> parseRequest(std::string_view buffer);

};


#endif