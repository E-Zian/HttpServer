add_executable(HttpServer "src/HttpServer.cpp" "header/HttpServer.h" "src/main.cpp" "src/Connection.cpp" "header/Connection.h"
        header/HttpTypes.h)

# Define Windows version for ASIO
target_compile_definitions(HttpServer PRIVATE _WIN32_WINNT=0x0A00)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET HttpServer PROPERTY CXX_STANDARD 20)
endif()

string(REPLACE "\\" "/" ASIO_HOME_FIXED "$ENV{ASIO_HOME}")
set(ASIO_INCLUDE_DIR "${ASIO_HOME_FIXED}/include")

target_include_directories(HttpServer PRIVATE ${ASIO_INCLUDE_DIR})
target_include_directories(HttpServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(HttpServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header)