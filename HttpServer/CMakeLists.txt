add_executable(HttpServer 
"src/HttpServer.cpp" 
"header/HttpServer.h" 
"src/main.cpp" 
"src/Connection.cpp" 
"header/Connection.h" 
"header/Helper.h" 
"src/Helper.cpp"   
"header/model/HttpTypes.h" 
"src/Router.cpp" 
"header/Router.h" 
"header/ResponseFactory.h" 
"src/ResponseFactory.cpp"
"src/controller/TestingController.cpp"
"header/controller/TestingController.h" 
"src/controller/ControllerBase.cpp" 
"header/controller/ControllerBase.h"
"header/Interface/Idispatcher.h"  
"header/database/database.h" 
"src/database/database.cpp"
 "src/repository/UserRepo.cpp" 
 "src/repository/PokemonRepo.cpp" 
 "header/repository/UserRepo.h" 
 "header/repository/PokemonRepo.h")

# Define Windows version for ASIO
target_compile_definitions(HttpServer PRIVATE _WIN32_WINNT=0x0A00)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET HttpServer PROPERTY CXX_STANDARD 20)
endif()


find_package(fmt CONFIG REQUIRED)
target_link_libraries(HttpServer PRIVATE fmt::fmt)

find_package(asio REQUIRED)
target_link_libraries(HttpServer PRIVATE asio::asio)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(HttpServer PRIVATE nlohmann_json::nlohmann_json)

find_package(SQLiteCpp CONFIG REQUIRED)
target_link_libraries(HttpServer PRIVATE SQLiteCpp)

target_include_directories(HttpServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(HttpServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header)
