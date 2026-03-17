#include "pch.h"
#include "Entity.h"

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