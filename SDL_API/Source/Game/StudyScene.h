#pragma once

#include "Core/ComponentTypes.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

#include "Core/Entity/Entity.h"

struct BoxCollider;
struct CircleCollider;

class StudyScene final : public Scene
{
public:
	StudyScene() = default;
	StudyScene(const StudyScene&) = delete;
	const StudyScene& operator=(const StudyScene&) = delete;
	~StudyScene() = default;

	void Initialize() override;
	bool Update(const float deltaTime) override;
	void Finalize()  override;

private:
	bool isCollisionEnter(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionStay(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionExit(const Entity& entity0, const Entity& entity1) const;

	std::pair<const Entity*, const Entity*> getCollidedEntityPair(const Entity& entity0, const Entity& entity1) const;
	void registerCollidedEntityPairs(const Entity& entity0, const Entity& entity1);
	
	Rect convertBoxColliderToWorldRect(const Transform& transform, const BoxCollider& boxCollider) const;
	Circle convertCircleColliderToWorldCircle(const Transform& transform, const CircleCollider& circleCollider) const;

	bool checkCollisionBoxBox(const Entity& entity0, const Entity& entity1);
	bool checkCollisionBoxCircle(const Entity& boxEntity, const Entity& circleEntity);

private:
	Texture mBoxTexture{};
	Texture mCircleTexture{};

	Entity mMainCamera{};
	Entity mPlayer{};
	Entity mRectMonster{};
	Entity mCircleMonster{};

	std::vector<std::pair<const Entity*, const Entity*>> mCollidedEntityPairs{};
	std::vector<std::pair<const Entity*, const Entity*>> mPreviousCollidedEntityPairs{};
};