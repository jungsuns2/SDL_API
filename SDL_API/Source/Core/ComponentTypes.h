#pragma once
#include "Component.h"
#include "Texture.h"

struct Transform final : public Component
{
	static constexpr uint32_t _ID = 0;
	Transform() : Component(&_ID) {}

	Position position{};
	Scale scale{ .width = 1.0f, .height = 1.0f };
	float angle{};
};

struct Material final : public Component
{
	static constexpr uint32_t _ID = 0;
	Material() : Component(&_ID) {}

	Texture* texture = nullptr;
};