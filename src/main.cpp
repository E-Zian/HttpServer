#include "Server.h"
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
#include <thread>

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
        DataBase db{"server.db"};
        asio::io_context io;

        asio::ssl::context sslContext{asio::ssl::context::tls_server};
        sslContext.use_certificate_chain_file("server.crt");
        sslContext.use_private_key_file("server.key", asio::ssl::context::pem);

        // Asset Manager
        // AssetManager& assetManager {AssetManager::getInstance()};
        // assetManager.loadAsset("assets/images/quagsire.ico","image/x-icon",);

        constexpr double maxBucketTokenCapacity{100000.0};
        constexpr double tokenRefillPerSec{ 100000.0 };
        RateLimiter rateLimiter(maxBucketTokenCapacity, tokenRefillPerSec);

        Router router{};

        // Repo Setup
        const PokemonRepo pokemonRepo{db};
        const UserRepo userRepo{db};

        TestingController testingController{router};
        PokemonController pokemonController{router, pokemonRepo};
        UserController userController{router, userRepo};

        int HttpPort{6767};
        Server<asio::ip::tcp::socket> httpServer(io, HttpPort, router, rateLimiter);

        int HttpsPort{6969};
        Server<asio::ssl::stream<asio::ip::tcp::socket> > httpsServer(io, HttpsPort, router, rateLimiter, &sslContext);

        std::vector<std::jthread> threads{};

        size_t maxThreads{ std::thread::hardware_concurrency() };
        if (maxThreads == 0) {
            maxThreads = 1;
        }
       
        for (size_t i{}; i+1 < maxThreads;i++) {
            threads.emplace_back([&]() {
                io.run();
                });
        }

        io.run();

        return 0;
    } catch (std::exception &e) {
        Helper::displayError("{}", e.what());
        return 1;
    }
}
