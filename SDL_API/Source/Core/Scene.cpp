#include "pch.h"
#include "Scene.h"
#include "Helper.h"

Helper* Scene::GetHelper() const
{
	return mHelper;
}

void Scene::SetHelper(Helper* helper)
{
	assert(helper != nullptr);

	mHelper = helper;
}