#pragma once

class Entity;

class EntityWorld final
{
public:
	EntityWorld() = default;
	EntityWorld(const EntityWorld& other) = delete;
	EntityWorld operator=(EntityWorld& other) = delete;
	~EntityWorld();

public:
	void AddEntity(Entity* entity);
	void Remove(Entity* entity);

	const std::vector<Entity*>& GetAllEntites() const;

private:
	std::vector<Entity*> mEntites{};
};

