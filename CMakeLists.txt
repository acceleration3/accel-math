cmake_minimum_required(VERSION 3.10)

project(accel-math CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(ACCEL_BUILD_TESTS ON)

add_library(accel-math INTERFACE)
target_include_directories(accel-math INTERFACE "include/")

if(ACCEL_BUILD_TESTS)
    add_subdirectory(tests)
endif()