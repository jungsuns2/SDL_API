#include "pch.h"
#include "Core/Entity/Entity.h"
#include "Core/Entity/EntityWorld.h"
#include "Helper.h"
#include "Scene.h"

Helper* Scene::GetHelper() const
{
	return mHelper;
}

void Scene::_SetHelper(Helper* helper)
{
	assert(helper != nullptr);

	mHelper = helper;
}

void Scene::SetEntityWorld(EntityWorld* entityWorld)
{
	assert(entityWorld != nullptr);

	mEntityWorld = entityWorld;
}

EntityWorld* Scene::GetEntityWorld() const
{
	return mEntityWorld;
}

void Scene::SetEntity(const std::vector<Entity*>* enties)
{
	assert(enties != nullptr);

	mEntites = enties;
}

const std::vector<Entity*>* Scene::_GetEntity() const
{
	 return mEntites;
}