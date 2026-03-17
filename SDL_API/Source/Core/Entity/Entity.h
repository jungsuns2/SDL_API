#pragma once

class Component;

class Entity final
{
public:
	Entity() = default;
	Entity(const Entity& other) = delete;
	Entity operator=(Entity& other) = delete;
	~Entity();

public:
	template<typename T>
	void AddComponent(const T& newComponent);

	template<typename T>
	T* GetComponent() const;

	template<typename T>
	bool HasComponent();

private:
	std::vector<Component*> mComponents{};
};