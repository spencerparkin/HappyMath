#include "HappyMath/Sphere.h"

using namespace HappyMath;

Sphere::Sphere()
{
	this->radius = 0.0;
}

Sphere::Sphere(const Vector3& center, double radius)
{
	this->center = center;
	this->radius = radius;
}

Sphere::Sphere(const Sphere& sphere)
{
	this->center = sphere.center;
	this->radius = sphere.radius;
}

/*virtual*/ Sphere::~Sphere()
{
}

void Sphere::operator=(const Sphere& sphere)
{
	this->center = sphere.center;
	this->radius = sphere.radius;
}

bool Sphere::ContainsPoint(const Vector3& point) const
{
	return (this->center - point).SquareLength() <= this->radius * this->radius;
}

void Sphere::Expand(const Vector3& point)
{
	double squareDistance = (this->center - point).SquareLength();
	
	if (squareDistance > this->radius * this->radius)
		this->radius = ::sqrt(squareDistance);
}