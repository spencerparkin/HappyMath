# HappyMath library by Spencer T. Parkin

cmake_minimum_required(VERSION 3.26)

project(HappyMath)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED HAPPYMATH_BUILD_TEST)
	set(HAPPYMATH_BUILD_TEST ON)
endif()

add_subdirectory(Library)

if(HAPPYMATH_BUILD_TEST)
	add_subdirectory(Test)
endif()