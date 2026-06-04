#include "Connection.h"
#include "Helper.h"
#include <iostream>
#include <asio.hpp>
#include <fmt/core.h>

namespace {
	std::optional<size_t> parseRequestForHeader(std::string_view buffer) {
		const std::string delimiter{ "\r\n\r\n" };
		const size_t delimiterPosition{ buffer.find(delimiter) };

		if (delimiterPosition == std::string::npos) {
			return std::nullopt;
		}

		return delimiterPosition;
	};

	std::string_view getHeaderLine(std::string_view header, size_t& startingPosition) {
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

	std::optional<std::pair<std::string, std::string>> parseHeaderLine(const std::string& headerLine) {
		const auto delimiter{ ":" };
		const size_t delimiterPosition{ headerLine.find(delimiter) };

		if (delimiterPosition == std::string::npos) {
			return std::nullopt;
		}
		return std::make_pair(headerLine.substr(0, delimiterPosition), Helper::trim(headerLine.substr(delimiterPosition + 1)));
	}

	std::optional<std::vector<std::string_view>> parseRequestLine(std::string_view requestLine) {
		std::vector<std::string_view> requestLineComponents{};
		size_t start{};
		while (start < requestLine.length()) {
			size_t end{ requestLine.find(' ',start) };

			if (end == std::string::npos) {
				requestLineComponents.push_back(requestLine.substr(start, requestLine.length() - start));
				break;
			}
			else {
				requestLineComponents.push_back(requestLine.substr(start, end - start));
				start = end + 1;
			}
		}
		if (requestLineComponents.size() < 3) {
			return std::nullopt;
		}
		return requestLineComponents;

	}
}

Connection::Connection(asio::io_context& io, tcp::socket&& connectionSocket, const int connectionId)
	: io_{ io },
	socket_{ std::move(connectionSocket) },
	connectionId_{ connectionId } {
}

Connection::pointer Connection::create(asio::io_context& io, tcp::socket&& connectionSocket, const int connectionId) {
	return pointer(new Connection(io, std::move(connectionSocket), connectionId));
}

Connection::~Connection() {
	Helper::displayMessage("Connection ID ({}) Disconnected\n", connectionId_);
}

asio::awaitable<void> Connection::startRead() {
	auto self{ shared_from_this() };
	std::array<char, 128> receivingBuffer{};

	std::optional<size_t> delimiterPosition{};
	asio::error_code ec;

	while (!delimiterPosition.has_value()) {
		const size_t len{
			co_await socket_.async_read_some(asio::buffer(receivingBuffer),
											 asio::redirect_error(asio::use_awaitable, ec))
		};

		if (ec) {
			Helper::displayError("Error Connection ID ({}) Request Failed: {}", connectionId_, ec.message());
			break;
		}

		requestReceived_.insert(requestReceived_.end(), receivingBuffer.begin(), receivingBuffer.begin() + len);

		delimiterPosition = parseRequestForHeader(std::string_view(requestReceived_.data(), requestReceived_.size()));
	}
	if (ec) {
		// send bad request
		co_return;
	}

	// Display Header
	Helper::displayMessage("Headers Received from Connection Id ({})\n\n{}\n", connectionId_, std::string_view(requestReceived_.data(), delimiterPosition.value()));

	request_.Header = std::string_view(requestReceived_.data(), delimiterPosition.value());

	size_t startingPosition{};

	const auto requestLine{ getHeaderLine(request_.Header, startingPosition) };

	auto parseRequestLineComponents{ parseRequestLine(requestLine) };

	if (!parseRequestLineComponents.has_value()) {
		Helper::displayError("Error Connection ID ({}) Request Failed: {}", connectionId_, "Malformed Request Line Received");

		// Send internal server error 
		co_return;
	};

	parsedRequest_.method = parseRequestLineComponents.value()[0];
	parsedRequest_.route = parseRequestLineComponents.value()[1];
	parsedRequest_.httpVersion = parseRequestLineComponents.value()[2];


	while (startingPosition != request_.Header.length()) {
		std::string headerLine{ getHeaderLine(request_.Header, startingPosition) };

		std::optional<std::pair<std::string, std::string>> headerPair{ parseHeaderLine(headerLine) };
		if (!headerPair.has_value()) {
			Helper::displayError("Error Connection ID ({}) Request Failed: {}{}", connectionId_, "Malformed header line : ", headerLine);
			// send bad request
			break;
		}
		parsedRequest_.header[headerPair.value().first] = headerPair.value().second;
	}


	// Has Body
	if (parsedRequest_.header.contains("Content-Length")) {
		const int contentLength{ std::stoi(parsedRequest_.header["Content-Length"]) };
		std::vector<char> receivingBodyBuffer(contentLength);

		size_t len{ co_await asio::async_read(socket_, asio::buffer(receivingBodyBuffer, contentLength), asio::use_awaitable) };

		requestReceived_.insert(requestReceived_.end(), receivingBodyBuffer.begin(), receivingBodyBuffer.begin() + len);

		// 4 represents the double carriage return and new line
		request_.Body = std::string_view(&requestReceived_[delimiterPosition.value() + static_cast<size_t>(4)],
			contentLength);
		parsedRequest_.body = request_.Body;
	}

	Helper::displayMessage("Connection ID ({}) Request Received\n", connectionId_);

	asio::co_spawn(io_, writeResponse(), asio::detached);
}


std::string Connection::generateDummyResponse() {
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
	auto self{ shared_from_this() };

	co_await asio::async_write(socket_, asio::buffer(generateDummyResponse()), asio::use_awaitable);
}
