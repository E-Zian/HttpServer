cmake_minimum_required(VERSION 3.10)

set(Sources
        UserControllerTest.cpp
        PokemonControllerTest.cpp
        main.cpp
)

add_executable(ServerTest ${Sources})

find_package(GTest CONFIG REQUIRED)

target_link_libraries(ServerTest PUBLIC
        ServerLib
        GTest::gtest
#        GTest::gtest_main
        GTest::gmock
)
target_include_directories(
        ServerTest PRIVATE include
)

add_test(
        NAME ServerTest COMMAND ServerTest
)