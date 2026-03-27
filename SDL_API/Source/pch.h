#pragma once

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

#include <iostream>
#include <chrono>
#include <algorithm>
#include <array>
#include <cassert>

struct Point
{
	float x;
	float y;
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
	[[nodiscard]] inline Point AddVector(const Point lhs, const Point rhs);
	[[nodiscard]] inline Point SubtractVector(const Point lhs, const Point rhs);
	[[nodiscard]] inline Point ScaleVector(const Point lhs, const float rhs);
	[[nodiscard]] inline float GetVectorLength(const Point vector);

	Point AddVector(const Point lhs, const Point rhs)
	{
		return { .x = lhs.x + rhs.x, .y = lhs.y + rhs.y };
	}

	Point SubtractVector(const Point lhs, const Point rhs)
	{
		return { .x = lhs.x - rhs.x, .y = lhs.y - rhs.y };
	}

	Point ScaleVector(const Point lhs, const float rhs)
	{
		return { .x = lhs.x * rhs, .y = lhs.y * rhs };
	}

	float GetVectorLength(const Point vector)
	{
		float length = sqrt(vector.x * vector.x + vector.y * vector.y);
		return length;
	}

	Point Sqrt(const Point lhs, const Point rhs)
	{
		return { .x = lhs.x * rhs.x, .y = lhs.y + rhs.y };
	}
}
