#pragma once

class Component
{
public:
	Component() = default;
	Component(const uint32_t id);
	Component(const Component&) = delete;
	Component& operator=(Component&) = delete;
	virtual ~Component() = 0;

public:
	uint32_t GetID() const;

private:
	uint32_t mID{};
};

