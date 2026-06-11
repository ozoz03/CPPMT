cmake_minimum_required(VERSION 3.20)

project(homework07 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# find_package(nlohmann_json 3.12.0 REQUIRED)
# set(JSON_Install OFF CACHE INTERNAL "")

# add_subdirectory(nlohmann_json)


include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(homework07 src/main.cpp)

target_link_libraries(homework07 PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(homework07 PUBLIC  ./include/interfaces ./include/dto ./include/core src)

# add_custom_command(
#     TARGET homework07 POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy
#             "${CMAKE_CURRENT_SOURCE_DIR}/data/config.json"
#             "$<TARGET_FILE_DIR:homework07>/config.json"
#     COMMENT "Copying config.json to the executable directory"
# )         

# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/config.json ${CMAKE_CURRENT_BINARY_DIR}/data/config.json COPYONLY)
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/targets.json ${CMAKE_CURRENT_BINARY_DIR}/data/targets.json COPYONLY)
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/ammo.json ${CMAKE_CURRENT_BINARY_DIR}/data/ammo.json COPYONLY)
# target_include_directories(homework07 PRIVATE  ./data)

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