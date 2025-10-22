cmake_minimum_required(VERSION 4.0)
project(C_arcade C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Werror -O2 -DDEBUG=1)

file(GLOB SRC_FILES src/*.c)

include_directories(include)

add_executable(c_arcade ${SRC_FILES}
        src/station_precision.c)

target_include_directories(c_arcade PRIVATE include)
