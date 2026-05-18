cmake_minimum_required(VERSION 3.20)
project(telemetry_check CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(SOURCES
    src/main.cpp
    src/telemetry.cpp
)

add_executable(telemetry_check ${SOURCES})

add_library(telemetry INTERFACE)
target_include_directories(telemetry INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(telemetry_check PRIVATE telemetry)

target_compile_options(telemetry_check PRIVATE -Wall -Wextra -pedantic -g)
