cmake_minimum_required(VERSION 3.20)

project(homework07 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(homework07 src/main.cpp src/config/ComponentFactory.cpp src/config/JsonConfigLoader.cpp src/providers/JsonTargetProvider.cpp src/solvers/AnalyticalSolver.cpp src/Utility.cpp)

target_link_libraries(homework07 PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(homework07 PUBLIC  ./include ./include/interfaces ./include/config ./include/dto ./include/providers ./src ./src/config ./src/providers ./src/solvers ./include/solvers)

# Expose the absolute data path to CMake variable context
set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

# Process config.hpp.in to generate config.hpp in the build directory
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.hpp"
    @ONLY
)

target_include_directories(homework07 PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_compile_options(homework07 PRIVATE -Wall -Wextra -pedantic)