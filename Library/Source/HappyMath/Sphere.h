#pragma once

#include "HappyMath/Vector3.h"

namespace HappyMath
{
	/**
	 * These are spheres in 3-dimensional space.
	 */
	class Sphere
	{
	public:
		Sphere();
		Sphere(const Vector3& center, double radius);
		Sphere(const Sphere& sphere);
		virtual ~Sphere();

		void operator=(const Sphere& sphere);

		/**
		 * Tell us if the given point is inside or on the surface of this sphere.
		 */
		bool ContainsPoint(const Vector3& point) const;

		/**
		 * Minimally expand this sphere to include the given point.
		 */
		void Expand(const Vector3& point);

	public:
		Vector3 center;
		double radius;
	};
}