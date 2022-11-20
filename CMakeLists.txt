cmake_minimum_required(VERSION 3.10)

project(accel-math CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_library(accel-math INTERFACE)
target_include_directories(accel-math INTERFACE "include/")

add_subdirectory(tests)