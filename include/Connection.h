#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#include "RateLimiter.h"
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

	static pointer create(tcp::socket&& connectionSocket, const size_t connectionId,const IDispatcher& dispatcher, RateLimiter& rateLimiter);

	~Connection();

	asio::awaitable<void> handleRequest();


private:
	tcp::socket socket_;
	size_t connectionId_;
	size_t totalRequests_;
	const IDispatcher& dispatcher_;
	const std::string clientIp_;
	RateLimiter& rateLimiter_;
	CheckLimitResult checkLimitResult_{};

	Connection(tcp::socket&& connectionSocket, const size_t connectionId,const IDispatcher& dispatcher, RateLimiter& rateLimiter);

	asio::awaitable<void> writeResponse(Response response);

	asio::awaitable<bool> processRequest();

};


#endif