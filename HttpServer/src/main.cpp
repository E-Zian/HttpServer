#include "HttpServer.h"
#include <asio.hpp>
#include <fmt/color.h>

int main()
{
	fmt::print(fmt::fg(fmt::color::green), "Green text\n");
	asio::io_context io;
	HttpServer server(io);

	io.run();
	return 0;
}