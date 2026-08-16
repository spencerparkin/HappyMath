#include "Surface.h"

using namespace HappyMath;

//-------------------------------------- SphereSurface --------------------------------------

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

//-------------------------------------- SphereSurface --------------------------------------

SphereSurface::SphereSurface(const Vector3& center, double radius)
{
	this->center = center;
	this->radius = radius;
}

/*virtual*/ SphereSurface::~SphereSurface()
{
}

/*virtual*/ bool SphereSurface::FindNearestPoint(const Vector3& point, Vector3& surfacePoint, Vector3& surfaceNormal) const
{
	Vector3 vector = point - this->center;
	double length = vector.Length();
	if (length == 0.0)
		return false;

	surfaceNormal = vector / length;
	surfacePoint = this->center + surfaceNormal * this->radius;
	return true;
}

/*virtual*/ bool SphereSurface::RayCast(const Ray& ray, Vector3& surfacePoint, Vector3& surfaceNormal) const
{
	double alpha;
	if (!ray.CastAgainstSphere(this->center, this->radius, alpha))
		return false;

	surfacePoint = ray.CalculatePoint(alpha);
	surfaceNormal = (surfacePoint - this->center).Normalized();
	return true;
}