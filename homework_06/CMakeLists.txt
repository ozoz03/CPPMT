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


# formatting
# find_package(fmt REQUIRED)
# target_link_libraries(my_app PRIVATE fmt::fmt)
# install(TARGETS my_app DESTINATION bin)
# 1. Locate the clang-format executable
find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_FORMAT_EXE)
    # 2. Get all source files for a target (or use globbing)
    file(GLOB_RECURSE ALL_SOURCE_FILES *.cpp *.hpp *.h *.cc)

    # 3. Add a custom 'format' target
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Formatting source files with clang-format..."
    )
endif()