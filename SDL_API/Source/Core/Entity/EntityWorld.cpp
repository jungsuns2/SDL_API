#include "pch.h"
#include "EntityWorld.h"
#include "Entity.h"

void EntityWorld::AddEntity(Entity* entity)
{
	assert(entity != nullptr);
	assert(std::find(mEntites.begin(), mEntites.end(), entity) == mEntites.end());

	mEntites.push_back(entity);
}

void EntityWorld::Remove(Entity* entity)
{
	//assert(entity != nullptr);
	//assert(std::find(mEntites.begin(), mEntites.end(), entity) == mEntites.end());

	//for (std::vector<Entity*>::iterator it = mEntites.begin(); it != mEntites.end(); ++it)
	//{
	//	Entity* removeEntity = *it;

	//	if (removeEntity == entity)
	//	{
	//		std::swap(removeEntity, mEntites.back());
	//		delete removeEntity;
	//		removeEntity = nullptr;
	//		mEntites.pop_back();
	//	}
	//}

	assert(entity != nullptr);

	std::vector<Entity*>::iterator it = std::find(mEntites.begin(), mEntites.end(), entity);
	if (it != mEntites.end())
	{
		std::swap(*it, mEntites.back());
		delete mEntites.back();
		mEntites.pop_back();
	}
}

const std::vector<Entity*>& EntityWorld::GetAllEntites() const
{
	return mEntites;
}