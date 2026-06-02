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

target_compile_options(homework07 PRIVATE -Wall -Wextra -pedantic)