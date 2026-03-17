#pragma once
#include "Component.h"
#include "Texture.h"

struct Transfrom final : public Component
{
	static constexpr unsigned int id = 0;
	Transfrom() : Component(&id) {}

	float x;
	float y;
	float z;
	float scale;
	float angle;
};

struct Material final : public Component
{
	static constexpr unsigned int id = 0;
	Material() : Component(&id) {}

	Texture* texture;
};