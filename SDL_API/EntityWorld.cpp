#include "pch.h"
#include "EntityWorld.h"
#include "Entity.h"

void EntityWorld::AddEntity(Entity* entity)
{
	assert(entity != nullptr);
	assert(std::find(mEntites.begin(), mEntites.end(), entity) == mEntites.end());

	mEntites.push_back(entity);
}

const std::vector<Entity*>& EntityWorld::GetAllEntites() const
{
	return mEntites;
}
