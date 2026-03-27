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

	Point operator+(const Point& other)
	{
		return { .x = x + other.x, .y = y + other.y };
	}

	Point operator+(const float other)
	{
		return { .x = x + other, .y = y + other };
	}

	Point operator-(const Point& other)
	{
		return { .x = x - other.x, .y = y - other.y };
	}

	Point operator*(const Point& other)
	{
		return { .x = x * other.x, .y = y * other.y };
	}

	Point operator*(const float other)
	{
		return { .x = x * other, .y = y * other };
	}

	Point operator+=(const Point& other)
	{
		x += other.x;
		y += other.y;

		return *this;
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
	[[nodiscard]] inline Point Sqrt(const Point lhs, const Point rhs);

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
