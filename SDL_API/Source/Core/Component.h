#pragma once

class Core;

class Component
{
public:
	Component() = default;
	Component(const uint32_t* id);
	~Component() = default;

public:
	const uint32_t* ID = 0;
};