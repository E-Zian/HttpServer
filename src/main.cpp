#include "HttpServer.h"
#include "controller/testingController.h"
#include "Router.h"
#include "repository/PokemonRepo.h"
#include "controller/PokemonController.h"
#include "AssetManager.h"
#include "controller/UserController.h"
#include "repository/UserRepo.h"
#include "database/database.h"
#include <asio.hpp>
#include <fmt/color.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    // To display color in terminal
    const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
    DataBase db{ "server.db" };
    asio::io_context io;

    // Asset Manager
    AssetManager& assetManager {AssetManager::getInstance()};
    // assetManager.loadAsset("assets/images/quagsire.ico","image/x-icon",);

    const Router router{};
    // Repo Setup
    const PokemonRepo pokemonRepo{db.get()};
    const UserRepo userRepo{db.get()};

    TestingController testingController{router};
    PokemonController pokemonController{router, pokemonRepo};
    UserController userController{router, userRepo};

    HttpServer server(io,6767,router);

    io.run();
    return 0;
}
