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
	inline Point AddVector(const Point lhs, const Point rhs);
	inline Point SubtractVector(const Point lhs, const Point rhs);
	inline Point ScaleVector(const Point lhs, const float rhs);

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
}
