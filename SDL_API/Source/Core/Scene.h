#pragma once
#include "Entity/EntityWorld.h"

class Helper;
class Core;
class Entity;

class Scene
{
public:
	Scene() = default;
	Scene(const Scene&) = delete;
	const Scene& operator=(Scene&) = delete;
	virtual ~Scene() = default;

public:
	virtual void Initialize() = 0;
	virtual bool Update(const float deltaTime) = 0;
	virtual void Finalize() = 0;

public:
	[[nodiscard]] Helper* GetHelper() const;

public:
	EntityWorld* GetEntityWorld();
	void _SetHelper(Helper* helper);

private:
	Helper* mHelper = nullptr;
	EntityWorld mEntityWorld{};
};