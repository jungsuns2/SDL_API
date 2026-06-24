#include "pch.h"
#include "EntityWorld.h"
#include "Entity.h"

Entity* EntityWorld::AddEntity(Entity* entity, std::source_location sourceLocation)
{
	assert(entity != nullptr);
	assert(std::find(mEntites.begin(), mEntites.end(), entity) == mEntites.end());

#if defined(_DEBUG)
	entity->SetSourceLocation(sourceLocation.file_name(), sourceLocation.line());
#endif

	mEntites.push_back(entity);
	
	return entity;
}

void EntityWorld::Remove(Entity* entity)
{
	assert(entity != nullptr);
	assert(std::find(mEntites.begin(), mEntites.end(), entity) != mEntites.end());

	for (std::vector<Entity*>::iterator it = mEntites.begin(); it != mEntites.end(); ++it)
	{
		Entity* removeEntity = *it;

		if (removeEntity == entity)
		{
			std::swap(removeEntity, mEntites.back());
			mEntites.pop_back();
			delete removeEntity;
			removeEntity = nullptr;

			return;
		}
	}
}

std::vector<Entity*>& EntityWorld::CleanRemovedEntities()
{
	for (int i = static_cast<int>(mEntites.size()) - 1; i >= 0; --i)
	{
		if (mEntites[i]->IsRemoved())
		{
			mEntites[i] = mEntites.back();
			mEntites.pop_back();
		}
	}

	return mEntites;
}

const std::vector<Entity*>& EntityWorld::GetAllEntites() const
{
	return mEntites;
}