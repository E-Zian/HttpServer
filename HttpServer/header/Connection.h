#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#include "HttpTypes.h"
#include "ResponseFactory.h"
#include <asio.hpp>
#include <memory>
#include <unordered_map>

class Connection :public std::enable_shared_from_this<Connection> {
public:
	using tcp = asio::ip::tcp;
	using pointer = std::shared_ptr<Connection>;

	static pointer create(tcp::socket&& connectionSocket,int connectionId);

	~Connection();

	asio::awaitable<void> startRead();

private:
	tcp::socket socket_;
	int connectionId_;
	std::vector<char> requestReceived_{};
	RequestObject request_;
	ParsedRequestObject parsedRequest_;

	Connection(tcp::socket&& connectionSocket, int connectionId);

	asio::awaitable<void> writeResponse(Response& response);

};


#endif