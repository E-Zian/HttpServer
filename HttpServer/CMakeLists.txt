add_executable(HttpServer "src/HttpServer.cpp" "header/HttpServer.h" "src/main.cpp" "src/Connection.cpp" "header/Connection.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET HttpServer PROPERTY CXX_STANDARD 20)
endif()

set(ASIO_INCLUDE_DIR $ENV{ASIO_HOME}/include)
target_include_directories(HttpServer PRIVATE ${ASIO_INCLUDE_DIR})
target_include_directories(HttpServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(HttpServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/header)