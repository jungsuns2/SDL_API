#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <iostream>
#include <chrono>
#include <algorithm>
#include <array>
#include <cassert>

struct Position
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