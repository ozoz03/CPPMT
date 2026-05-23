cmake_minimum_required(VERSION 3.20)

project(project07 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(project07 src/main.cpp)

target_include_directories(project07 PUBLIC include/interfaces include/dto)

target_compile_options(project07 PRIVATE -Wall -Wextra -pedantic)