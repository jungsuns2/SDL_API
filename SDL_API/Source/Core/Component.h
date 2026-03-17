#pragma once

class Component
{
public:
	Component(const uint32_t id);

	uint32_t GetID() const;

private:
	uint32_t mID{};
};

