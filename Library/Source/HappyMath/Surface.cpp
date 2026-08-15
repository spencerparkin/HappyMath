#include "Surface.h"

using namespace HappyMath;

Surface::Surface()
{
}

/*virtual*/ Surface::~Surface()
{
}

/*virtual*/ bool Surface::FindNearestPoint(const Vector3& point, Vector3& surfacePoint, Vector3& surfaceNormal) const
{
	return false;
}

/*virtual*/ bool Surface::RayCast(const Ray& ray, Vector3& surfacePoint, Vector3& surfaceNormal) const
{
	return false;
}