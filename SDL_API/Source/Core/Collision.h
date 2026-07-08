#pragma once
#include "ComponentTypes.h"

class Collision final
{
public:
	Collision() = default;
	Collision(const Collision&) = delete;
	const Collision operator=(Collision&) = delete;
	~Collision() = default;

	[[nodiscard]] static Collision& Get();

public:
	[[nodiscard]] std::pair<const Entity*, const Entity*> GetCollidedEntityPair(const Entity& entity0, const Entity& entity1) const;
	void RegisterCollidedEntityPairs(const Entity& entity0, const Entity& entity1);

	const std::vector<std::pair<const Entity*, const Entity*>>& GetCollidedEntityPairs() const;
	const std::vector<std::pair<const Entity*, const Entity*>>& GetPreviousCollidedEntityPairs() const;

	void UpdateEntityPairs();

	bool CheckCollisionBoxBox(const Entity& entity0, const Entity& entity1);
	bool CheckCollisionBoxCircle(const Entity& boxEntity, const Entity& circleEntity);
	bool CheckCollisionBoxLine(const Entity& boxEntity, const Entity& lineEntity);
	bool CheckCollisionCircleCircle(const Entity& entity0, const Entity& entity1);
	bool CheckCollisionCircleLine(const Entity& circleEntity, const Entity& lineEntity);

	[[nodiscard]] Quad ConvertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const;
	[[nodiscard]] Circle ConvertCircleColliderToWorldCircle(const Transform& transform, const CircleCollider& circleCollider) const;
	[[nodiscard]] Line ConvertLineColliderToWorldLine(const Transform& transform, const LineCollider& lineCollider) const;

private:
	std::vector<std::pair<const Entity*, const Entity*>> mCollidedEntityPairs{};
	std::vector<std::pair<const Entity*, const Entity*>> mPreviousCollidedEntityPairs{};
};

