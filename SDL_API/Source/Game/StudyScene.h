#pragma once

#include "Core/ComponentTypes.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

#include "Core/Entity/Entity.h"

//struct BoxCollider;
//struct CircleCollider;
//struct LineCollider;
//
//class StudyScene final : public Scene
//{
//public:
//	StudyScene() = default;
//	StudyScene(const StudyScene&) = delete;
//	const StudyScene& operator=(const StudyScene&) = delete;
//	~StudyScene() = default;
//
//	void Initialize() override;
//	bool Update(const float deltaTime) override;
//	void Finalize()  override;
//
//private:
//	bool isCollisionEnter(const Entity& entity0, const Entity& entity1) const;
//	bool isCollisionStay(const Entity& entity0, const Entity& entity1) const;
//	bool isCollisionExit(const Entity& entity0, const Entity& entity1) const;
//
//	std::pair<const Entity*, const Entity*> getCollidedEntityPair(const Entity& entity0, const Entity& entity1) const;
//	void registerCollidedEntityPairs(const Entity& entity0, const Entity& entity1);
//	
//	Rect convertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const;
//	Circle convertCircleColliderToWorldCircle(const Transform& transform, const CircleCollider& circleCollider) const;
//	Line convertLineColliderToWorldLine(const Transform& transform, const LineCollider& lineCollider) const;
//
//	bool checkCollisionBoxBox(const Entity& entity0, const Entity& entity1);
//	bool checkCollisionBoxCircle(const Entity& boxEntity, const Entity& circleEntity);
//	bool checkCollisionBoxLine(const Entity& boxEntity, const Entity& lineEntity);
//	bool checkCollisionCircleCircle(const Entity& entity0, const Entity& entity1);
//	bool checkCollisionCircleLine(const Entity& circleEntity, const Entity& lineEntity);
//
//private:
//	Texture mBoxTexture{};
//	Texture mCircleTexture{};
//
//	Entity mMainCamera{};
//	Entity mRectPlayer{};
//	Entity mCirclePlayer{};
//	Entity mRectMonster{};
//	Entity mCircleMonster{};
//	Entity mLineMonster{};
//
//	std::vector<std::pair<const Entity*, const Entity*>> mCollidedEntityPairs{};
//	std::vector<std::pair<const Entity*, const Entity*>> mPreviousCollidedEntityPairs{};
//};