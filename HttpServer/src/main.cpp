#include "HttpServer.h"
#include <asio.hpp>

int main()
{
	asio::io_context io;
	HttpServer server(io);

	io.run();
	return 0;
}