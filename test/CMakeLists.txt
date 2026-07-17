cmake_minimum_required(VERSION 3.10)

set(Sources
        serverTest.cpp

)

add_executable(ServerTest ${Sources} ${Headers})

find_package(GTest CONFIG REQUIRED)

target_link_libraries(ServerTest PRIVATE
        ServerLib
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
)
target_include_directories(
        ServerTest PRIVATE include
)

add_test(
        NAME ServerTest COMMAND ServerTest
)