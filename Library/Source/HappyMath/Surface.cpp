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

/*virtual*/ Surface::Side Surface::WhichSide(const Vector3& point, double tolerance /*= 1e-6*/) const
{
	return Side::NEITHER;
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

/*virtual*/ Surface::Side SphereSurface::WhichSide(const Vector3& point, double tolerance /*= 1e-6*/) const
{
	double squareDistance = (this->center - point).SquareLength();
	
	if (squareDistance <= this->radius * this->radius - tolerance)
		return Side::INSIDE;

	if (squareDistance >= this->radius * this->radius + tolerance)
		return Side::OUTSIDE;

	return Side::NEITHER;
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
	Vector3 spinePoint = this->CalcNearestSpinePoint(point);

	surfaceNormal = point - spinePoint;
	surfaceNormal.Normalize();

	surfacePoint = spinePoint + surfaceNormal * this->girthRadius;

	return true;
}

Vector3 EllipticalDonutSurface::CalcNearestSpinePoint(const Vector3& point) const
{
	auto func = [this, &point](double t) -> double
		{
			Vector3 delta = point - this->CalcSpinePoint(t);
			return delta.SquareLength();
		};

	double t = FindExtrema(func, ExtremaType::Minimum, Interval(0.0, 2.0 * M_PI), 16);

	return this->CalcSpinePoint(t);
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

/*virtual*/ Surface::Side EllipticalDonutSurface::WhichSide(const Vector3& point, double tolerance /*= 1e-6*/) const
{
	Vector3 spinePoint = this->CalcNearestSpinePoint(point);

	double squareDistance = (point - spinePoint).SquareLength();

	if (squareDistance <= this->girthRadius * this->girthRadius - tolerance)
		return Side::INSIDE;

	if (squareDistance >= this->girthRadius * this->girthRadius + tolerance)
		return Side::OUTSIDE;

	return Side::NEITHER;
}

//-------------------------------------- UnionSurface --------------------------------------

UnionSurface::UnionSurface(Surface* surfaceA, Surface* surfaceB)
{
	this->surfaceA = surfaceA;
	this->surfaceB = surfaceB;
}

/*virtual*/ UnionSurface::~UnionSurface()
{
	delete this->surfaceA;
	delete this->surfaceB;
}

/*virtual*/ bool UnionSurface::FindNearestPoint(const Vector3& point, Vector3& surfacePoint, Vector3& surfaceNormal) const
{
	Vector3 workingPoint = point;

	int maxIterations = 1000;
	
	for (int i = 0; i < maxIterations; i++)
	{
		Vector3 surfacePointA, surfaceNormalA;
		Vector3 surfacePointB, surfaceNormalB;

		if (!this->surfaceA->FindNearestPoint(workingPoint, surfacePointA, surfaceNormalA))
			return false;

		if (!this->surfaceB->FindNearestPoint(workingPoint, surfacePointB, surfaceNormalB))
			return false;

		Surface::Side surfacePointASide = this->surfaceB->WhichSide(surfacePointA);
		Surface::Side surfacePointBSide = this->surfaceA->WhichSide(surfacePointB);

		if (surfacePointASide != Surface::Side::INSIDE)
		{
			if (surfacePointBSide != Surface::Side::INSIDE)
			{
				double squareDistanceA = (surfacePointA - workingPoint).SquareLength();
				double squareDistanceB = (surfacePointB - workingPoint).SquareLength();

				if (squareDistanceA < squareDistanceB)
				{
					surfacePoint = surfacePointA;
					surfaceNormal = surfaceNormalA;
				}
				else
				{
					surfacePoint = surfacePointB;
					surfaceNormal = surfaceNormalB;
				}

				return true;
			}
			else
			{
				surfacePoint = surfacePointA;
				surfaceNormal = surfaceNormalA;

				return true;
			}
		}
		else
		{
			if (surfacePointBSide != Surface::Side::INSIDE)
			{
				surfacePoint = surfacePointB;
				surfaceNormal = surfaceNormalB;

				return true;
			}
			else
			{
				Vector3 newWorkingPoint = (surfacePointA + surfacePointB) / 2.0;

				if ((newWorkingPoint - workingPoint).SquareLength() == 0.0)
					return false;

				workingPoint = newWorkingPoint;
			}
		}
	}

	return false;
}