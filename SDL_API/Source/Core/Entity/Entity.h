#pragma once
#include "Core/Component.h"

class Entity final
{
public:
	Entity() = default;
	Entity(const Entity&) = delete;
	Entity& operator=(Entity&) = delete;
	~Entity();

public:
	template<typename T>
	void AddComponent(const T& newComponent);

	template<typename T>
	T* GetComponent() const;

	template<typename T>
	bool HasComponent() const;

private:
	std::vector<Component*> mComponents{};
};

template<typename T>
void Entity::AddComponent(const T& newComponent)
{
#if defined(_DEBUG)
	for (Component* component : mComponents)
	{
		if (component->ID == &newComponent._ID)
		{
			assert(false and "중복된 컴포넌트입니다.");
			return;
		}
	}
#endif

	mComponents.push_back(new T(newComponent));
}
template<typename T>
T* Entity::GetComponent() const
{
	for (Component* component : mComponents)
	{
		if (component->ID == &T::_ID)
		{
			return static_cast<T*>(component);
		}
	}

	assert(false && "없는 컴포넌트입니다.");
	return nullptr;
}
template<typename T>
bool Entity::HasComponent() const
{
	for (Component* component : mComponents)
	{
		if (component->ID == &T::_ID)
		{
			return true;
		}
	}

	return false;
};