#pragma once

class Component
{
public:
	Component(const uint32_t id);
	virtual ~Component() = 0;

	uint32_t GetID() const;

private:
	uint32_t mID{};
};

