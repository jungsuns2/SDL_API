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

EntityWorld* Scene::_GetEntityWorld() const
{
	return mEntityWorld;
}

const std::vector<Entity*>* Scene::_GetEntity() const
{
	 return mEntites;
}