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
#include "Helper.h"
#include <asio.hpp>
#include <fmt/color.h>
#include <asio/ssl.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    try {


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

    // For tls
    asio::ssl::context sslContext{asio::ssl::context::tls_server};
    sslContext.use_certificate_chain_file("server.crt");
    sslContext.use_private_key_file("server.key", asio::ssl::context::pem);

    // Asset Manager
    // AssetManager& assetManager {AssetManager::getInstance()};
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
    HttpServer server(io, portToListen,router,rateLimiter,sslContext);

    io.run();
    return 0;
    }catch (std::exception &e) {
        Helper::displayError("{}", e.what());
    return 1;

    }
}
