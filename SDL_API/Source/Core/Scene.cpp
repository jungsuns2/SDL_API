#include "pch.h"
#include "Core/ComponentTypes.h"
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

void Scene::SetType(const eSceneType type)
{
	mSceneType = type;
}

EntityWorld* Scene::GetEntityWorld()
{
	return &mEntityWorld;
}

const EntityWorld* Scene::GetEntityWorld() const
{
	return &mEntityWorld;
}
