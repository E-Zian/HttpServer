#include "HttpServer.h"
#include "controller/TestingController.h"
#include "Router.h"
#include "repository/PokemonRepo.h"
#include "controller/PokemonController.h"
#include "RateLimiter.h"
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

    double maxbucketTokenCapacity{ 100.0 };
    double tokenRefillPerSec{ 1.0 };
    RateLimiter rateLimiter(maxbucketTokenCapacity, tokenRefillPerSec);

    const Router router{};

    // Repo Setup
    const PokemonRepo pokemonRepo{db.get()};
    const UserRepo userRepo{db.get()};

    TestingController testingController{router};
    PokemonController pokemonController{router, pokemonRepo};
    UserController userController{router, userRepo};

    int portToListen{ 6767 };
    HttpServer server(io, portToListen,router,rateLimiter);

    io.run();
    return 0;
}
