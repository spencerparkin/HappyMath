#include "Surface.h"
#include "Function.h"

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

//-------------------------------------- EllipticalDonutSurface --------------------------------------

EllipticalDonutSurface::EllipticalDonutSurface()
{
	this->transform.SetIdentity();
	this->A = 1.0;
	this->B = 1.0;
	this->girthRadius = 0.25;
}

/*virtual*/ EllipticalDonutSurface::~EllipticalDonutSurface()
{
}

/*virtual*/ bool EllipticalDonutSurface::FindNearestPoint(const Vector3& point, Vector3& surfacePoint, Vector3& surfaceNormal) const
{
	auto func = [this, &point](double t) -> double
		{
			Vector3 delta = point - this->CalcSpinePoint(t);
			return delta.SquareLength();
		};

	double t = FindExtrema(func, ExtremaType::Minimum, Interval(0.0, 2.0 * M_PI), 16);

	Vector3 spinePoint = this->CalcSpinePoint(t);

	surfaceNormal = point - spinePoint;
	surfaceNormal.Normalize();

	surfacePoint = spinePoint + surfaceNormal * this->girthRadius;

	return true;
}

/*virtual*/ bool EllipticalDonutSurface::RayCast(const Ray& ray, Vector3& surfacePoint, Vector3& surfaceNormal) const
{
	// STPTODO: Write this one day maybe.
	return false;
}

Vector3 EllipticalDonutSurface::CalcSpinePoint(double t) const
{
	Vector3 point;

	point.x = this->A * ::cos(t);
	point.y = this->B * ::sin(t);

	point = this->transform.TransformPoint(point);

	return point;
}