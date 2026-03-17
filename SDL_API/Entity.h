#pragma once

class Component;

class Entity
{
public:
	Entity() = default;
	Entity(const Entity& other) = delete;
	Entity operator=(Entity& other) = delete;
	~Entity() = default;

public:
	void AddComponent(const Component& newComponent);

private:
	std::vector<Component*> mComponents{};
};

