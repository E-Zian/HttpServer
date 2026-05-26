#include "Connection.h"
#include "Helper.h"
#include <iostream>
#include <asio.hpp>


std::string Connection::serverShutdownMessage_{ "The connection is closed due to the server shutting down" };

Connection::Connection(asio::io_context& io, tcp::socket&& connectionSocket, const int connectionId)
	: socket_{ std::move(connectionSocket) },
	connectionId_{ connectionId } {
}


Connection::pointer Connection::create(asio::io_context& io, tcp::socket&& connectionSocket, const int connectionId) {
	return pointer(new Connection(io, std::move(connectionSocket), connectionId));
}

Connection::~Connection() {
	std::cout << "Connection ID (" << connectionId_ << ") Disconnected" << "\n";
}

asio::awaitable<void> Connection::startRead() {
	auto self = shared_from_this();

	while (true) {

		asio::error_code ec;
		const size_t len{
			co_await socket_.async_read_some(asio::buffer(receivingBuffer_),asio::redirect_error(asio::use_awaitable, ec))
		};

		if (ec == asio::error::eof) {
			std::optional<RequestObject> request{
				Connection::parseRequest(std::string_view(requestReceived_.data(), requestReceived_.size()))
			};

			if (request.has_value()) {
				// Has header and body
				request_ = request.value();

				// Process header
				size_t startingPosition{};
				requestLine_ = getHeaderLine(request_.Header, startingPosition);

				while (startingPosition != request_.Header.length()) {
					std::string headerLine{ getHeaderLine(request_.Header, startingPosition) };
					parseHeaderLine(headerLine);

				}

				std::cout << "Connection ID (" << connectionId_ << ") Request Received" << "\n";
			}
			else {

				std::cout << "Connection ID (" << connectionId_ << ") Request Failed" << "\n";
			}

			break;
		}

		std::cout.write(receivingBuffer_.data(), len);
		requestReceived_.insert(requestReceived_.end(), receivingBuffer_.begin(), receivingBuffer_.begin() + len);


		if (ec) {
			std::cout << "Error Connection ID (" << connectionId_ << ") Failed Read: " << ec << "\n";
		}

	}
}

std::optional<RequestObject> Connection::parseRequest(std::string_view buffer) {
	const std::string delimiter{ "\r\n\r\n" };
	const size_t delimiterPosition{ buffer.find(delimiter) };

	// Need add some sort of security here i think to check \n\r\n\r
	if (delimiterPosition == std::string::npos) {
		return std::nullopt;
	}

	return RequestObject{
		buffer.substr(0, delimiterPosition), buffer.substr(delimiterPosition + delimiter.length())
	};
}

std::string_view Connection::getHeaderLine(std::string_view header, size_t& startingPosition) {
	const char* delimiter{ "\r\n" };
	size_t delimiterPosition{ header.find(delimiter, startingPosition) };

	size_t oldStartingPosition{ startingPosition };

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

void Connection::parseHeaderLine(std::string headerLine) {
	const char* delimiter{ ":" };
	// add a secrity injection of \r\n here??
	size_t delimiterPosition{ headerLine.find(delimiter) };

	if (delimiterPosition == std::string::npos) {
		std::cout << "\":\" not found in header line : "<<headerLine << "\n";
		return;
	}
	parsedHeader_[headerLine.substr(0, delimiterPosition)] = trim(headerLine.substr(delimiterPosition + 1));

}