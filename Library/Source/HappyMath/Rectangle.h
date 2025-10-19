#pragma once

#include "HappyMath/Vector2.h"

namespace HappyMath
{
	/**
	 * These are axis-aligned rectangles in the XY-plane.
	 */
	class HAPPY_MATH_API Rectangle
	{
	public:
		Rectangle();
		Rectangle(const Rectangle& rectangle);
		Rectangle(const Vector2& minCorner, const Vector2& maxCorner);

		void operator=(const Rectangle& rectangle);
		void operator+=(const Vector2& vector);
		void operator-=(const Vector2& vector);

		bool IsValid() const;
		void MakeInvalid();
		double GetWidth() const;
		double GetHeight() const;
		double GetAspectRatio() const;
		Vector2 GetCenter() const;
		bool ContainsPoint(const Vector2& point) const;
		bool Intersect(const Rectangle& rectangleA, const Rectangle& rectangleB);
		void ExpandToIncludePoint(const Vector2& point);
		void ExpandToIncludeRect(const Rectangle& rectangle);
		void ScaleHorizontallyToMatchAspectRatio(double aspectRatio);
		void ScaleVerticallyToMatchAspectRatio(double aspectRatio);
		void ExpandToMatchAspectRatio(double aspectRatio);
		void ContractToMatchAspectRatio(double aspectRatio);
		void ScaleHorizontally(double scale);
		void ScaleVertically(double scale);
		void AddHorizontalMargin(double margin);
		void AddVerticalMargin(double margin);
		Vector2 PointToUVs(const Vector2& point) const;
		Vector2 PointFromUVs(const Vector2& uv) const;

	public:
		Vector2 minCorner, maxCorner;
	};
}