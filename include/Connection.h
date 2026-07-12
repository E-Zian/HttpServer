#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#include "RateLimiter.h"
#include "Interface/IDispatcher.h"
#include "model/HttpTypes.h"
#include "HttpRequestParser.h"
#include <asio.hpp>
#include <memory>
#include <asio/ssl.hpp>
#include <asio/ssl.hpp>

namespace Constants {
	inline constexpr size_t MAX_REQUEST_PER_CONNECTION{ 100 };
	inline constexpr size_t maxHeaderSize{8192};
	inline constexpr size_t maxBodySize{1024 * 1024};
}


template <typename Stream>
class Connection :public std::enable_shared_from_this<Connection<Stream>> {
public:
	using tcp = asio::ip::tcp;
	using pointer = std::shared_ptr<Connection>;

	static pointer create(tcp::socket&& connectionSocket,size_t connectionId,const IDispatcher& dispatcher, RateLimiter& rateLimiter, asio::ssl::context& sslContext);

	~Connection();

	asio::awaitable<void> handleRequest();


private:
	const std::string clientIp_;
	Stream socket_;
	// asio::ssl::stream<tcp::socket> socket_;
	const size_t connectionId_;
	size_t totalRequests_;
	const IDispatcher& dispatcher_;
	RateLimiter& rateLimiter_;
	CheckLimitResult checkLimitResult_{};

	Connection(tcp::socket&& connectionSocket,size_t connectionId,const IDispatcher& dispatcher, RateLimiter& rateLimiter, asio::ssl::context& sslContext);

	asio::awaitable<void> writeResponse(Response response);

	asio::awaitable<bool> processRequest();

};


#endif