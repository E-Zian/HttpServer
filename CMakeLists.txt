cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Server")

set(Headers 
        "include/Server.h"
        "include/Connection.h"
        "include/Helper.h"
        "include/model/HttpTypes.h"
        "include/Router.h"
        "include/ResponseFactory.h"
        "include/controller/TestingController.h"
        "include/controller/ControllerBase.h"
        "include/Interface/IDispatcher.h"
        "include/Interface/IRouteRegistrar.h"
        "include/database/database.h"
        "include/repository/UserRepo.h"
        "include/repository/PokemonRepo.h" 
        "include/model/UserModel.h" 
        "include/model/PokemonModel.h" 
        "include/controller/UserController.h" 
        "include/controller/PokemonController.h" 
        include/controller/HomeController.h
        include/RateLimiter.h 
        "include/Interface/IPokemonRepo.h" 
        "include/Interface/IUserRepo.h"
        include/AssetManager.h 
        "include/HttpRequestParser.h"

)

set(SourcesNoMain
        "src/Server.cpp"
        "src/Connection.cpp"
        "src/Helper.cpp"
        "src/Router.cpp"
        "src/ResponseFactory.cpp"
        "src/controller/TestingController.cpp"
        "src/controller/ControllerBase.cpp"
        "src/database/database.cpp"
        "src/repository/UserRepo.cpp"
        "src/repository/PokemonRepo.cpp"
        "src/controller/UserController.cpp" 
        "src/controller/PokemonController.cpp"
        src/controller/HomeController.cpp
        src/AssetManager.cpp
        "src/HttpRequestParser.cpp" 
        src/RateLimiter.cpp
)

add_library(ServerLib STATIC ${Headers} ${SourcesNoMain})

if(WIN32)
    # Define Windows version for ASIO
    target_compile_definitions(ServerLib PUBLIC _WIN32_WINNT=0x0A00)
endif()

target_compile_features(ServerLib PUBLIC cxx_std_20)

enable_testing()

find_package(asio REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)

find_package(SQLiteCpp CONFIG REQUIRED)

find_package(OpenSSL REQUIRED)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(ServerLib PUBLIC
    fmt::fmt
    asio::asio
    SQLiteCpp
    nlohmann_json::nlohmann_json
    OpenSSL::SSL OpenSSL::Crypto
)

target_include_directories(ServerLib PUBLIC include)

add_executable(Server src/main.cpp)

add_custom_command(TARGET Server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/server.crt ${CMAKE_SOURCE_DIR}/server.key
        $<TARGET_FILE_DIR:Server>)


target_link_libraries(Server PRIVATE ServerLib)
add_subdirectory(test)