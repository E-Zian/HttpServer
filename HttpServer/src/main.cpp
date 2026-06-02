#include "HttpServer.h"
#include <asio.hpp>
#include <fmt/color.h>

int main() {
#ifdef _WIN32
#include <windows.h>
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    asio::io_context io;
    HttpServer server(io);

    io.run();
    return 0;
}
