#pragma once

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <algorithm>
#include <array>
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
};

struct Scale
{
	float width;
	float height;
};

struct Color
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
};

namespace Math
{
	[[nodiscard]] inline float GetVectorLength(const Point vector);

	float GetVectorLength(const Point vector)
	{
		float length = sqrt(vector.x * vector.x + vector.y * vector.y);
		return length;
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