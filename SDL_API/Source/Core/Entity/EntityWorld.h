#pragma once

class Entity;

class EntityWorld final
{
public:
	EntityWorld() = default;
	EntityWorld(const EntityWorld&) = delete;
	EntityWorld& operator=(EntityWorld&) = delete;
	~EntityWorld() = default;

public:
	Entity* AddEntity(Entity* entity, std::source_location sourceLocation = std::source_location::current());
	void Remove(Entity* entity);

public:
	std::vector<Entity*>& CleanRemovedEntities();

	const std::vector<Entity*>& GetAllEntities() const;

private:
	std::vector<Entity*> mEntites{};
};