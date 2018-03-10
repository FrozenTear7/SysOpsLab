# cmake_minimum_required(VERSION <specify CMake version here>)
project(SysOpsLab)

set(CMAKE_CXX_STANDARD 11)

add_executable(SysOpsLab lab1/main.c)
add_library(SysOpsLab STATIC lab1/ex1shared.h lab1/ex1.h)
add_library(SysOpsLab SHARED lab1/library1shared.h)