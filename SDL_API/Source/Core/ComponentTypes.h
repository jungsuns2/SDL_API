#pragma once
#include "Component.h"
#include "Texture.h"

struct Transfrom final : public Component
{
	static constexpr uint32_t id = 0;
	Transfrom() : Component(id) {}

	Position position;
	Scale scale;
	float angle;
};

struct Material final : public Component
{
	static constexpr unsigned int id = 0;
	Material() : Component(id) {}

	Texture* texture;
};