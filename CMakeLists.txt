# HappyMath library by Spencer T. Parkin

cmake_minimum_required(VERSION 3.26)

project(HappyMath)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Library)
add_subdirectory(Test)