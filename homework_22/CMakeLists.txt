cmake_minimum_required(VERSION 3.20)

project(homework22 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

find_library(GPIOD_LIBRARY NAMES gpiod)
find_path(GPIOD_INCLUDE_DIR NAMES gpiod.h)

# libgpiod-dev — системна залежність (не FetchContent), її може не бути на
# машині, з якої конфігурують увесь репозиторій (не лише homework_22). Замість
# FATAL_ERROR, що зупинив би конфігурацію решти homework_*, просто пропускаємо
# ціль з попередженням.
if(NOT GPIOD_LIBRARY OR NOT GPIOD_INCLUDE_DIR)
    message(WARNING "homework22: libgpiod (dev headers) not found — install libgpiod-dev; skipping target")
    return()
endif()

add_executable(homework22
    src/main.cpp
    src/UartLink.cpp
    src/GpioLink.cpp
    src/Autopilot.cpp
    src/BalisticMath.cpp)

target_include_directories(homework22 PRIVATE
    ./include ${GPIOD_INCLUDE_DIR})

target_link_libraries(homework22 PRIVATE
    Threads::Threads ${GPIOD_LIBRARY})

target_compile_options(homework22 PRIVATE -Wall -Wextra -pedantic)
