set(HAPPY_MATH_TEST_SOURCES
	Source/Test.cpp
	Source/Test.h
)

add_executable(HappyMathTest
	${HAPPY_MATH_TEST_SOURCES}
)

target_link_libraries(HappyMathTest PRIVATE
	HappyMath
)