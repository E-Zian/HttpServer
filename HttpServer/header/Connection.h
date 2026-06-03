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

	static std::string serverShutdownMessage_;

	~Connection();

	tcp::socket& getSocket() {
		return socket_;
	}

	asio::awaitable<void> startRead();
	asio::awaitable<void> shutdown();

private:
	asio::io_context& io_;
	tcp::socket socket_;
	int connectionId_;
	std::array<char, 128> receivingBuffer_{};
	std::vector<char> requestReceived_{};
	RequestObject request_;
	ParsedRequestObject parsedRequest_;

	Connection(asio::io_context& io, tcp::socket&& connectionSocket, int connectionId);

	static std::optional<size_t> parseRequestForHeader(std::string_view buffer);

	static std::string_view getHeaderLine(std::string_view header,size_t& startingPosition);

	void parseHeaderLine(const std::string& headerLine);

	bool parseRequestLine(std::string_view requestLine);

	asio::awaitable<void> writeResponse();

	std::string generateResponse();

	std::string generateDummyResponse();
};


#endif