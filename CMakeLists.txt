cmake_minimum_required(VERSION 3.9)
project(SysOps)

set(CMAKE_CXX_STANDARD 11)

add_library(LibblocksStatic STATIC libblocks.c)
add_library(LibblocksShared SHARED libblocks.c)

add_executable(MainStatic main.c)
add_executable(MainShared main.c)

target_link_libraries(MainStatic LibblocksStatic)
target_link_libraries(MainShared LibblocksShared)