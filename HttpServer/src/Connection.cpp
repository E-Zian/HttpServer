#include "Connection.h"
#include "Helper.h"
#include <iostream>
#include <asio.hpp>


std::string Connection::serverShutdownMessage_{ "The connection is closed due to the server shutting down" };

Connection::Connection(asio::io_context& io, tcp::socket&& connectionSocket, const int connectionId)
	: io_{io},
	socket_{ std::move(connectionSocket) },
	connectionId_{ connectionId }
{
}

Connection::pointer Connection::create(asio::io_context& io, tcp::socket&& connectionSocket, const int connectionId) {
	return pointer(new Connection(io, std::move(connectionSocket), connectionId));
}

Connection::~Connection() {
	std::cout << "Connection ID (" << connectionId_ << ") Disconnected" << "\n";
}

asio::awaitable<void> Connection::startRead() {
	auto self { shared_from_this()};

	while (true) {

		asio::error_code ec;
		const size_t len{
			co_await socket_.async_read_some(asio::buffer(receivingBuffer_),asio::redirect_error(asio::use_awaitable, ec))
		};

		std::cout.write(receivingBuffer_.data(), len);
		requestReceived_.insert(requestReceived_.end(), receivingBuffer_.begin(), receivingBuffer_.begin() + len);

		std::optional<size_t> delimiterPosition = parseRequestForHeader(std::string_view(requestReceived_.data(), requestReceived_.size()));

		if (!delimiterPosition.has_value()) {
			continue;
		}

		request_.Header = std::string_view(requestReceived_.data(), delimiterPosition.value());

		size_t startingPosition{};
		requestLine_ = getHeaderLine(request_.Header, startingPosition);

		while (startingPosition != request_.Header.length()) {
			std::string headerLine{ getHeaderLine(request_.Header, startingPosition) };
			parseHeaderLine(headerLine);

		}

		if (ec) {
			std::cout << "Error Connection ID (" << connectionId_ << ") Request Failed: " << ec << "\n";
			break;
		}

		std::cout << "Connection ID (" << connectionId_ << ") Request Received" << "\n";

		// Has Body
		if (parsedHeader_.contains("Content-Length")) {
			const int contentLength{std::stoi(parsedHeader_["Content-Length"])};

			// 4 represents the double carriage return and new line
			request_.Body = std::string_view(&receivingBuffer_[delimiterPosition.value() + static_cast<size_t>(4)], contentLength);

		}

		asio::co_spawn(io_,writeResponse(),asio::detached);
		break;
	}
}


std::optional<size_t> Connection::parseRequestForHeader(std::string_view buffer) {
	const std::string delimiter{ "\r\n\r\n" };
	const size_t delimiterPosition{ buffer.find(delimiter) };

	if (delimiterPosition == std::string::npos) {
		return std::nullopt;
	}

	return delimiterPosition;
};


std::string_view Connection::getHeaderLine(std::string_view header, size_t& startingPosition) {
	const auto delimiter{ "\r\n" };
	const size_t delimiterPosition{ header.find(delimiter, startingPosition) };

	const size_t oldStartingPosition{ startingPosition };

	if (delimiterPosition == std::string::npos) {
		startingPosition = header.length();
		return header.substr(oldStartingPosition);
	}

	startingPosition = delimiterPosition + 2;
	return header.substr(oldStartingPosition, delimiterPosition - oldStartingPosition);

}


asio::awaitable<void> Connection::shutdown() {
	auto self = shared_from_this();
	co_await socket_.async_write_some(asio::buffer(receivingBuffer_), asio::use_awaitable);
	socket_.close();
};

void Connection::parseHeaderLine(const std::string& headerLine) {
	const auto delimiter{ ":" };
	// add a security injection of \r\n here??
	const size_t delimiterPosition{ headerLine.find(delimiter) };

	if (delimiterPosition == std::string::npos) {
		std::cout << "\":\" not found in header line : " << headerLine << "\n";
		return;
	}
	parsedHeader_[headerLine.substr(0, delimiterPosition)] = trim(headerLine.substr(delimiterPosition + 1));
}

std::string Connection::generateResponse() {
	std::string response =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 27\r\n"
	"Connection: close\r\n"
	"\r\n"
	"<html>Hello World!</html>";

	return response;
}

asio::awaitable<void> Connection::writeResponse() {
	auto self {shared_from_this()};

	co_await asio::async_write(socket_, asio::buffer(generateResponse()),asio::use_awaitable);
}
