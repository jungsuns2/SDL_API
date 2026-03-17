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