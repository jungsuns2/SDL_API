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

protected:
	void SetEntityWorld(EntityWorld* entityWorld);
	EntityWorld* GetEntityWorld() const;

	void SetEntity(const std::vector<Entity*>* enties);

	void _SetHelper(Helper* helper);
	const std::vector<Entity*>* _GetEntity() const;

private:
	Helper* mHelper = nullptr;

	EntityWorld* mEntityWorld = nullptr;
	const std::vector<Entity*>* mEntites{};

	friend Core;
};