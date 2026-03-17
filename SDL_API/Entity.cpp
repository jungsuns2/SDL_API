#include "pch.h"
#include "Entity.h"
#include "Component.h"

void Entity::AddComponent(const Component& newComponent)
{
	mComponents.push_back(new Component(newComponent));
}
