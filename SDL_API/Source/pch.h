#pragma once

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <iostream>
#include <source_location>

struct Point
{
	float x;
	float y;

	Point operator+(const Point& other) const
	{
		return { .x = x + other.x, .y = y + other.y };
	}

	Point& operator+=(const Point& other)
	{
		x += other.x;		
		y += other.y;

		return *this;
	}

	Point& operator+=(const float other)
	{
		x += other;		
		y += other;

		return *this;
	}

	Point operator-(const Point& other) const
	{
		return { .x = x - other.x, .y = y - other.y };
	}

	Point& operator-=(const Point& other)
	{
		x -= other.x;
		y -= other.y;

		return *this;
	}

	Point operator*(const Point& other) const
	{
		return { .x = x * other.x, .y = y * other.y };
	}

	Point operator*(const float other) const
	{
		return { .x = x * other, .y = y * other };
	}

	Point& operator*=(const Point& other)
	{
		x *= other.x;
		y *= other.y;

		return *this;
	}

	Point operator/(const float other) const
	{
		assert(other != 0.0f and "Division Error");

		return { .x = x / other, .y = y / other };
	}
};

struct Scale
{
	float width;
	float height;

	Scale operator*(const Scale other) const
	{
		return { .width = width * other.width, .height = height * other.height };
	}

	Scale operator*(const float other) const
	{
		return { .width = width * other, .height = height * other };
	}
};

struct Range
{
	float min;
	float max;
};

struct Rect
{
	float left;
	float top;
	float right;
	float bottom;
};

struct Line
{
	Point point0;
	Point point1;
};

struct Circle
{
	Point center;
	float radius;
};

struct Quad
{
	Point leftTop;
	Point rightTop;
	Point leftBottom;
	Point rightBottom;
};

namespace Math
{
	inline float GetVectorLength(const Point vector)
	{
		float length = sqrt(vector.x * vector.x + vector.y * vector.y);
		return length;
	}

	[[nodiscard]] inline float CrossProduct2D(const Point point0, const Point point1, const Point point2)
	{
		float ccw = (point1.x - point0.x) * (point2.y - point0.y) - (point2.x - point0.x) * (point1.y - point0.y);
		return ccw;
	}

	[[nodiscard]] inline float DotProduct2D(const Point lhs, const Point rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}

	[[nodiscard]] inline Point NormalizeVector(const Point vector)
	{
		float length = GetVectorLength(vector);

		if (length <= 0.0001f)
		{
			return { 0.0f, 0.0f };
		}

		Point result{ .x = vector.x / length, .y = vector.y / length };
		return result;
	}

	[[nodiscard]] inline Point RotatePoint(const Point point, const float degree)
	{
		const float radian = degree * (3.141592f / 180.0f);
		const float cosTheta = cos(radian);
		const float sinTheta = sin(radian);

		Point result =
		{
			.x = cosTheta * point.x - sinTheta * point.y,
			.y = sinTheta * point.x + cosTheta * point.y,
		};

		return result;
	}

	[[nodiscard]] inline Point LerpVector(const Point a, const Point b, const float t)
	{
		Point ab = b - a;
		Point result = a + (ab * t);

		return result;
	}
}

[[nodiscard]] inline Point operator+(const Point& p, const Scale& s)
{
	return { p.x + s.width, p.y + s.height };
}

[[nodiscard]] inline Point operator-(const Point& p, const Scale& s)
{
	return { p.x - s.width, p.y - s.height };
}