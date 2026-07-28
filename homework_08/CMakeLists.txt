cmake_minimum_required(VERSION 3.20)

project(homework08 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

find_package(Threads REQUIRED)

# ДЗ18: опційний ThreadSanitizer — cmake ... -DHW08_TSAN=ON
option(HW08_TSAN "Build homework08 with ThreadSanitizer" OFF)

add_executable(homework08
    src/main.cpp
    src/config/ComponentFactory.cpp
    src/config/JsonConfigLoader.cpp
    src/providers/ThreadSafeTargetProvider.cpp
    src/physics/DronePhysics.cpp
    src/solvers/AnalyticalSolver.cpp
    src/solvers/TableSolver.cpp
    src/solvers/BallisticTable.cpp
    src/Utility.cpp
    src/state/StateMoving.cpp
    src/state/StateAccelerating.cpp
    src/state/StateStopped.cpp
    src/state/StateTurning.cpp
    src/state/StateDecelerating.cpp
    src/MissionProcessor.cpp)

target_link_libraries(homework08 PRIVATE nlohmann_json::nlohmann_json Threads::Threads)

target_include_directories(homework08 PUBLIC
    ./include ./include/interfaces ./include/config ./include/dto
    ./include/providers ./include/physics ./include/concurrency
    ./include/solvers ./include/state
    ./src ./src/config ./src/providers ./src/physics ./src/solvers ./src/state)

# Expose the absolute data path to CMake variable context
set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.hpp"
    @ONLY)

target_include_directories(homework08 PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_compile_options(homework08 PRIVATE -Wall -Wextra -pedantic)

if(HW08_TSAN)
    target_compile_options(homework08 PRIVATE -fsanitize=thread -g)
    target_link_options(homework08 PRIVATE -fsanitize=thread)
endif()
