cmake_minimum_required(VERSION 3.9)
project(SysOps)

set(CMAKE_CXX_STANDARD 11)

add_library(Libblocks STATIC libblocks.c)

add_executable(Main main.c)

target_link_libraries(Main Libblocks)