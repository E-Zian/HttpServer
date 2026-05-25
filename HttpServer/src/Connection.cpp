#include "Connection.h"
#include <iostream>
#include <asio.hpp>
std::string Connection::serverShutdownMessage_{"The connection is closed due to the server shutting down"};

Connection::Connection(asio::io_context &io, tcp::socket &&connectionSocket, const int connectionId)
    : socket_{std::move(connectionSocket)},
      connectionId_{connectionId} {
}


Connection::pointer Connection::create(asio::io_context &io, tcp::socket &&connectionSocket, const int connectionId) {
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
                co_await socket_.async_read_some(asio::buffer(receivingBuffer_),
                                                 asio::redirect_error(asio::use_awaitable, ec))
            };

            if (ec == asio::error::eof) {
                std::optional<ParsedRequest> request{
                    Connection::parseRequest(std::string_view(requestReceived_.data(), requestReceived_.size()))
                };

                if (request.has_value()) {
                    std::cout << "Connection ID (" << connectionId_ << ") Request Received" << "\n";
                } else {
                    std::cout << "Connection ID (" << connectionId_ << ") Request Failed" << "\n";
                }

                break;
            }

            std::cout.write(receivingBuffer_.data(), len);
            requestReceived_.insert(requestReceived_.end(), receivingBuffer_.begin(), receivingBuffer_.begin()+len);


            if (ec) {
                std::cout << "Error Connection ID (" << connectionId_ << ") Failed Read: " << ec << "\n";
            }

    }
}

std::optional<ParsedRequest> Connection::parseRequest(std::string_view buffer) {
    const std::string delimiter{"\r\n\r\n"};
    const size_t delimiterPosition{buffer.find(delimiter)};

    if (delimiterPosition == std::string::npos) {
        return std::nullopt;
    }

    return ParsedRequest{
        buffer.substr(0, delimiterPosition), buffer.substr(delimiterPosition + delimiter.length(), buffer.length())
    };
}

asio::awaitable<void> Connection::shutdown() {
    auto self = shared_from_this();
    co_await socket_.async_write_some(asio::buffer(receivingBuffer_), asio::use_awaitable);
    socket_.close();
};
