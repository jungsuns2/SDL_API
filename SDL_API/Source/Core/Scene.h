#pragma once
#include "Entity/EntityWorld.h"

class Helper;
class Core;
class Entity;
struct Camera;

class Scene
{
public:
	enum class eSceneType
	{
		Start,
		Main,
		Continue,
		Count
	};

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
	EntityWorld* GetEntityWorld();
	const EntityWorld* GetEntityWorld() const;

	[[nodiscard]] Helper* GetHelper() const;
	void _SetHelper(Helper* helper);

public:
	void SetType(const eSceneType type);

private:
	Helper* mHelper = nullptr;
	EntityWorld mEntityWorld{};

	eSceneType mSceneType{};
};