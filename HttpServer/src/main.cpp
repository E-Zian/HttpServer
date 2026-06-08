#include "HttpServer.h"
#include "controller/testingController.h"
#include "Router.h"
#include <asio.hpp>
#include <fmt/color.h>
#include <windows.h>
int main() {
    // To display color in terminal
    const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // ------------------------------------

    asio::io_context io;

    const Router router{};
    TestingController testingController{router};

    HttpServer server(io,6767,router);

    io.run();
    return 0;
}
