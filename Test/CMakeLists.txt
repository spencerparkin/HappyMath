# CMakeLists.txt for HappyMath test program.

find_package(OpenGL REQUIRED)

include(FetchContent)

FetchContent_Declare(
	SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.2.16
)

FetchContent_MakeAvailable(SDL3)

set(HAPPY_MATH_TEST_SOURCES
	Source/Test.cpp
	Source/Test.h
	Source/App.cpp
	Source/App.h
)

add_executable(HappyMathTest
	${HAPPY_MATH_TEST_SOURCES}
)

target_compile_definitions(HappyMathTest PRIVATE
	_USE_MATH_DEFINES
)

target_link_libraries(HappyMathTest PRIVATE
	HappyMath
	SDL3::SDL3
	OpenGL::GL
)