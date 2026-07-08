# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("HttpServer")

add_executable(HttpServer
        "src/HttpServer.cpp"
        "include/HttpServer.h"
        "src/main.cpp"
        "src/Connection.cpp"
        "include/Connection.h"
        "include/Helper.h"
        "src/Helper.cpp"
        "include/model/HttpTypes.h"
        "src/Router.cpp"
        "include/Router.h"
        "include/ResponseFactory.h"
        "src/ResponseFactory.cpp"
        "src/controller/TestingController.cpp"
        "include/controller/TestingController.h"
        "src/controller/ControllerBase.cpp"
        "include/controller/ControllerBase.h"
        "include/Interface/Idispatcher.h"
        "include/Interface/IRouteRegistrar.h"
        "include/database/Database.h"
        "src/database/database.cpp"
        "src/repository/UserRepo.cpp"
        "src/repository/PokemonRepo.cpp"
        "include/repository/UserRepo.h"
        "include/repository/PokemonRepo.h" "include/model/UserModel.h" "include/model/PokemonModel.h" "include/controller/UserController.h" "include/controller/PokemonController.h" "src/controller/UserController.cpp" "src/controller/PokemonController.cpp"
        src/controller/HomeController.cpp
        include/controller/HomeController.h
        src/AssetManager.cpp
        include/AssetManager.h "src/HttpRequestParser.cpp" "include/HttpRequestParser.h"
        src/RateLimiter.cpp
        include/RateLimiter.h "include/Interface/IPokemonRepo.h" "include/Interface/IUserRepo.h")

# Define Windows version for ASIO
target_compile_definitions(HttpServer PRIVATE _WIN32_WINNT=0x0A00)

target_compile_features(HttpServer PRIVATE cxx_std_20)   # cleanest — sets AND requires C++20
set_target_properties(HttpServer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(HttpServer PRIVATE fmt::fmt)

find_package(asio REQUIRED)
target_link_libraries(HttpServer PRIVATE asio::asio)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(HttpServer PRIVATE nlohmann_json::nlohmann_json)

find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(HttpServer PRIVATE SQLiteCpp)

target_include_directories(HttpServer PRIVATE src)
target_include_directories(HttpServer PRIVATE include)

