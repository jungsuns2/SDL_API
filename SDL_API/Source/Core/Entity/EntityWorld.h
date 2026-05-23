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
	void AddEntity(Entity* entity, std::source_location sourceLocation = std::source_location::current());
	void Remove(Entity* entity);

	const std::vector<Entity*>& GetAllEntites() const;

private:
	std::vector<Entity*> mEntites{};
};