#include "HappyMath/Rectangle.h"

using namespace HappyMath;

Rectangle::Rectangle()
{
}

Rectangle::Rectangle(const Rectangle& rectangle)
{
	this->minCorner = rectangle.minCorner;
	this->maxCorner = rectangle.maxCorner;
}

Rectangle::Rectangle(const Vector2& minCorner, const Vector2& maxCorner)
{
	this->minCorner = minCorner;
	this->maxCorner = maxCorner;
}

bool Rectangle::IsValid() const
{
	if (this->minCorner.x > this->maxCorner.x)
		return false;

	if (this->minCorner.y > this->maxCorner.y)
		return false;

	return true;
}

void Rectangle::MakeInvalid()
{
	this->minCorner.x = std::numeric_limits<double>::max();
	this->minCorner.y = std::numeric_limits<double>::max();
	this->maxCorner.x = std::numeric_limits<double>::min();
	this->maxCorner.y = std::numeric_limits<double>::min();
}

void Rectangle::operator=(const Rectangle& rectangle)
{
	this->minCorner = rectangle.minCorner;
	this->maxCorner = rectangle.maxCorner;
}

void Rectangle::operator+=(const Vector2& vector)
{
	this->minCorner += vector;
	this->maxCorner += vector;
}

void Rectangle::operator-=(const Vector2& vector)
{
	this->minCorner -= vector;
	this->maxCorner -= vector;
}

double Rectangle::GetWidth() const
{
	return this->maxCorner.x - this->minCorner.x;
}

double Rectangle::GetHeight() const
{
	return this->maxCorner.y - this->minCorner.y;
}

double Rectangle::GetAspectRatio() const
{
	return this->GetWidth() / this->GetHeight();
}

Vector2 Rectangle::GetCenter() const
{
	return (this->minCorner + this->maxCorner) / 2.0;
}

bool Rectangle::ContainsPoint(const Vector2& point) const
{
	return
		this->minCorner.x <= point.x && point.x <= this->maxCorner.x &&
		this->minCorner.y <= point.y && point.y <= this->maxCorner.y;
}

bool Rectangle::Intersect(const Rectangle& rectangleA, const Rectangle& rectangleB)
{
	// TODO: Write this.
	return false;
}

void Rectangle::ExpandToIncludePoint(const Vector2& point)
{
	this->minCorner.x = HM_MIN(this->minCorner.x, point.x);
	this->minCorner.y = HM_MIN(this->minCorner.y, point.y);
	this->maxCorner.x = HM_MAX(this->maxCorner.x, point.x);
	this->maxCorner.y = HM_MAX(this->maxCorner.y, point.y);
}

void Rectangle::ExpandToIncludeRect(const Rectangle& rectangle)
{
	this->ExpandToIncludePoint(rectangle.minCorner);
	this->ExpandToIncludePoint(rectangle.maxCorner);
}

void Rectangle::ScaleHorizontallyToMatchAspectRatio(double aspectRatio)
{
	double delta = (this->GetHeight() * aspectRatio - this->GetWidth()) / 2.0;
	this->minCorner.x -= delta;
	this->minCorner.x += delta;
}

void Rectangle::ScaleVerticallyToMatchAspectRatio(double aspectRatio)
{
	double delta = (this->GetWidth() / aspectRatio - this->GetHeight()) / 2.0;
	this->minCorner.y -= delta;
	this->maxCorner.y += delta;
}

void Rectangle::ExpandToMatchAspectRatio(double aspectRatio)
{
	if (aspectRatio > this->GetAspectRatio())
		this->ScaleVerticallyToMatchAspectRatio(aspectRatio);
	else
		this->ScaleHorizontallyToMatchAspectRatio(aspectRatio);
}

void Rectangle::ContractToMatchAspectRatio(double aspectRatio)
{
	if (aspectRatio < this->GetAspectRatio())
		this->ScaleVerticallyToMatchAspectRatio(aspectRatio);
	else
		this->ScaleHorizontallyToMatchAspectRatio(aspectRatio);
}

Vector2 Rectangle::PointToUVs(const Vector2& point) const
{
	Vector2 uv;
	uv.x = (point.x - this->minCorner.x) / this->GetWidth();
	uv.y = (point.y - this->minCorner.y) / this->GetHeight();
	return uv;
}

Vector2 Rectangle::PointFromUVs(const Vector2& uv) const
{
	Vector2 point;
	point.x = this->minCorner.x + uv.x * this->GetWidth();
	point.y = this->minCorner.y + uv.y * this->GetHeight();
	return point;
}