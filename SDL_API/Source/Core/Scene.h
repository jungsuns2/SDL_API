#pragma once

class Helper;
class Core;
class Entity;
class EntityWorld;

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
	EntityWorld* _GetEntityWorld() const;
	void _SetHelper(Helper* helper);

protected:
	void SetEntityWorld(EntityWorld* entityWorld);

private:
	Helper* mHelper = nullptr;

	EntityWorld* mEntityWorld = nullptr;
	const std::vector<Entity*>* mEntites{};

	friend Core;
};