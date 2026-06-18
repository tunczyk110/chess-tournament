cmake_minimum_required(VERSION 3.13)
project(chess-tournament LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(chess-tournament src/main.cpp)
target_compile_options(chess-tournament PUBLIC -std=gnu++26 -Wall -Wpedantic -Wextra -Werror)
add_subdirectory(src)
