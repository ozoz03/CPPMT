cmake_minimum_required(VERSION 3.20)

enable_testing()
project(ballistics CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ballistics src/main.cpp)

target_include_directories(ballistics PRIVATE include)
target_compile_options(ballistics PRIVATE -Wall -Wextra -pedantic)
add_library(ballisticsLib src/ballistics.cpp)
target_include_directories(ballisticsLib PUBLIC include)
target_compile_options(ballisticsLib PRIVATE -Wall -Wextra -pedantic)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

# доступні таргети 
# GTest::gtest
# GTest::gtest_main

# тест
add_subdirectory(tests)
