#pragma once

#include "Vector3.h"
#include "Transform.h"
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
		 * If there is more than one such point, or no such point for that matter, then fail.
		 */
		virtual bool FindNearestPoint(const Vector3& point, Vector3& surfacePoint, Vector3& surfaceNormal) const;

		/**
		 * Find and return the nearest point hit by the given ray, if any.
		 */
		virtual bool RayCast(const Ray& ray, Vector3& surfacePoint, Vector3& surfaceNormal) const;

		enum Side
		{
			NEITHER,
			INSIDE,
			OUTSIDE
		};

		/**
		 * Determine which side a given point is on.  Some surfaces don't have a clear
		 * inside and outside, but if they do, then you can return that here.
		 */
		virtual Side WhichSide(const Vector3& point, double tolerance = 1e-6) const;
	};

	/**
	 * This is a simple example of a surface.
	 */
	class SphereSurface : public Surface
	{
	public:
		SphereSurface(const Vector3& center, double radius);
		virtual ~SphereSurface();

		virtual bool FindNearestPoint(const Vector3& point, Vector3& surfacePoint, Vector3& surfaceNormal) const override;
		virtual bool RayCast(const Ray& ray, Vector3& surfacePoint, Vector3& surfaceNormal) const override;
		virtual Side WhichSide(const Vector3& point, double tolerance = 1e-6) const override;

		Vector3 center;
		double radius;
	};

	/**
	 * This is a somewhat more complicated example of a surface.
	 */
	class EllipticalDonutSurface : public Surface
	{
	public:
		EllipticalDonutSurface();
		virtual ~EllipticalDonutSurface();

		virtual bool FindNearestPoint(const Vector3& point, Vector3& surfacePoint, Vector3& surfaceNormal) const override;
		virtual bool RayCast(const Ray& ray, Vector3& surfacePoint, Vector3& surfaceNormal) const override;
		virtual Side WhichSide(const Vector3& point, double tolerance = 1e-6) const override;

		Vector3 CalcSpinePoint(double t) const;
		Vector3 CalcNearestSpinePoint(const Vector3& point) const;

		Transform transform;
		double A, B;
		double girthRadius;
	};

	/**
	 * Here we try to form the surface that is the union of two other surfaces.
	 * This can make sense if each surface has a clearly defined inside and outside.
	 */
	class UnionSurface : public Surface
	{
	public:
		UnionSurface(Surface* surfaceA, Surface* surfaceB);
		virtual ~UnionSurface();

		virtual bool FindNearestPoint(const Vector3& point, Vector3& surfacePoint, Vector3& surfaceNormal) const override;

	private:
		Surface* surfaceA;
		Surface* surfaceB;
	};
}