#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#include "Interface/IDispatcher.h"
#include "model/HttpTypes.h"
#include <asio.hpp>
#include <memory>

class Connection :public std::enable_shared_from_this<Connection> {
public:
	using tcp = asio::ip::tcp;
	using pointer = std::shared_ptr<Connection>;

	static pointer create(tcp::socket&& connectionSocket,int connectionId,const IDispatcher& dispatcher);

	~Connection();

	asio::awaitable<void> startRead();

private:
	tcp::socket socket_;
	int connectionId_;
	std::string requestReceived_{};
	ParsedRequestObject parsedRequest_{};
	const IDispatcher& dispatcher_;

	Connection(tcp::socket&& connectionSocket, int connectionId,const IDispatcher& dispatcher);

	asio::awaitable<void> writeResponse(const Response& response);

};


#endif