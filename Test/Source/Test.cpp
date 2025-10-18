#include "Test.h"
#include "HappyMath/Vector3.h"

int main(int argc, char** argv)
{
	using namespace HappyMath;

	Vector3 vecA(1.0, 2.0, 3.0);
	Vector3 vecB(4.0, 5.0, 6.0);

	Vector3 vecC = vecA + vecB;

	return 0;
}