#pragma once

#include "Vector3.h"
#include "Ray.h"

namespace HappyMath
{
	/**
	 * These are implicitly or parametrically defined manifolds with arbitrary topologies.
	 * They are mathematically precise subsets of 3D space.  This allows approximations of
	 * them to be "sensed" using this class's interface.
	 */
	class Surface
	{
	public:
		Surface();
		virtual ~Surface();

		/**
		 * Find and return the point on this surface nearest the given point.
		 * There can, of course, be more than one such point in some cases.
		 * Just return one of them.
		 */
		virtual bool FindNearestPoint(const Vector3& point, Vector3& surfacePoint, Vector3& surfaceNormal) const;

		/**
		 * Find and return the nearest point hit by the given ray, if any.
		 */
		virtual bool RayCast(const Ray& ray, Vector3& surfacePoint, Vector3& surfaceNormal) const;
	};
}