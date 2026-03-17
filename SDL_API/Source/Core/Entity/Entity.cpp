#include "pch.h"
#include "Entity.h"
#include "Core/Component.h"

Entity::~Entity()
{
	for (Component* component : mComponents)
	{
		if (component != nullptr)
		{
			delete component;
			component = nullptr;
		}
	}
}

template<typename T>
void Entity::AddComponent(const T& newComponent)
{
#if defined(_DEBUG)
	for (Component* component : mComponents)
	{
		if (component->GetID() == newComponent.GetID())
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
		if (component->GetID() == &T::GetID())
		{
			return static_cast<T*> (component);
		}
	}

	assert(false && "없는 컴포넌트입니다.");
	return nullptr;
}

template<typename T>
bool Entity::HasComponent()
{
	for (Component* component : mComponents)
	{
		if (component->GetID() == &T::GetID())
		{
			return true;
		}

		return false;
	}
}