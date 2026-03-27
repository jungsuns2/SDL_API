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

EntityWorld* Scene::GetEntityWorld() 
{
	return &mEntityWorld;
}