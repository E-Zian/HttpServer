#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#include "Interface/IDispatcher.h"
#include "model/HttpTypes.h"
#include "HttpRequestParser.h"
#include <asio.hpp>
#include <memory>

namespace Constants {
	inline constexpr size_t MAX_REQUEST_PER_CONNECTION{ 100 };
}

class Connection :public std::enable_shared_from_this<Connection> {
public:
	using tcp = asio::ip::tcp;
	using pointer = std::shared_ptr<Connection>;

	static pointer create(tcp::socket&& connectionSocket,int connectionId,const IDispatcher& dispatcher);

	~Connection();

	asio::awaitable<void> handleRequest();


private:
	tcp::socket socket_;
	size_t connectionId_;
	size_t totalRequests_;
	const IDispatcher& dispatcher_;

	Connection(tcp::socket&& connectionSocket, int connectionId,const IDispatcher& dispatcher);

	asio::awaitable<void> writeResponse(const Response& response);

	asio::awaitable<bool> processRequest();

};


#endif