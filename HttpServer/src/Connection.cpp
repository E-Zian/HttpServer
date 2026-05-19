#include "Connection.h"
#include <iostream>

std::string Connection::serverShutdownMessage{ "The connection is closed due to the server shutting down" };

Connection::Connection(asio::io_context& io, tcp::socket&& connectionSocket, int connectionId)
	:socket_{ std::move(connectionSocket) },
	connectionId{ connectionId }
{

}

Connection::pointer Connection::create(asio::io_context& io, tcp::socket&& connectionSocket,int connectionId) {
	return pointer(new Connection(io, std::move(connectionSocket), connectionId));
}

Connection::~Connection() {
	std::cout << "Connection ID ("<< connectionId<<") Disconnected" << "\n";
}

asio::awaitable<void> Connection::startRead() {
	auto self = shared_from_this();

	while (true) {
		try {
			size_t len{ co_await asio::async_read(socket_, asio::buffer(receivingBuffer),asio::transfer_at_least(1), asio::use_awaitable) };

			while (len > 0 && (receivingBuffer[len - 1] == '\r' || receivingBuffer[len - 1] == '\n')) {
				--len;
			}
			if (len > 0) {
				std::cout << "Connection ID (" << connectionId << ") : ";

				std::cout.write(receivingBuffer.data(), len) << "\n";
			}

		}
		catch (std::exception& ex) {

			std::cout << "Error Connection ID (" << connectionId << ") Failed Read : "<<ex.what()<<"\n";
			break;
		}

	}
}


asio::awaitable<void> Connection::shutdown() {
	auto self = shared_from_this();

	co_await socket_.async_write_some()
};

