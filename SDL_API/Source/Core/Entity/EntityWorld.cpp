#include "pch.h"
#include "EntityWorld.h"
#include "Entity.h"

void EntityWorld::AddEntity(Entity* entity, std::source_location sourceLocation)
{
	assert(entity != nullptr);
	assert(std::find(mEntites.begin(), mEntites.end(), entity) == mEntites.end());

#if defined(_DEBUG)
	entity->SetSourceLocation(sourceLocation.file_name(), sourceLocation.line());
#endif

	mEntites.push_back(entity);
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
	uint32_t removeIndex = 0;

	for (uint32_t i = 0; i < mEntites.size(); ++i)
	{
		const Entity& entity = *mEntites[i];
		if (entity.IsRemoved())
		{
			removeIndex = i;
			mEntites[removeIndex] = mEntites[mEntites.size() - 1];
			mEntites.pop_back();

			--i;
		}
	}

	return mEntites;
}

const std::vector<Entity*>& EntityWorld::GetAllEntites() const
{
	return mEntites;
}