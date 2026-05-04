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

struct Point
{
	float x;
	float y;

	[[nodiscard]] Point operator+(const Point other) const
	{
		return { .x = x + other.x, .y = y + other.y };
	}

	[[nodiscard]] Point operator+(const float other) const
	{
		return { .x = x + other, .y = y + other };
	}

	[[nodiscard]] Point operator-(const Point other) const
	{
		return { .x = x - other.x, .y = y - other.y };
	}

	[[nodiscard]] Point operator*(const Point other) const
	{
		return { .x = x * other.x, .y = y * other.y };
	}

	[[nodiscard]] Point operator*(const float other) const
	{
		return { .x = x * other, .y = y * other };
	}

	[[nodiscard]] Point operator/(const float other) const
	{
		assert(other != 0.0f and "Division Error");

		return { .x = x / other, .y = y / other };
	}
};

struct Scale
{
	float width;
	float height;
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

struct Ellipse
{
	Point point;
	float radiusX;
	float radiusY;
};

namespace Math
{
	[[nodiscard]] inline float GetVectorLength(const Point vector);

	float GetVectorLength(const Point vector)
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
}

[[nodiscard]] inline Point operator+(const Point& p, const Scale& s)
{
	return { p.x + s.width, p.y + s.height };
}

[[nodiscard]] inline Point operator-(const Point& p, const Scale& s)
{
	return { p.x - s.width, p.y - s.height };
}