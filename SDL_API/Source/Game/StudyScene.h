#pragma once

#include "Core/Clip.h"
#include "Core/ComponentTypes.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

#include "Core/Entity/Entity.h"

#include "GameComponentTypes.h"


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
	Point getScreenMousePosition() const;

private:
	Texture mspwanTexture{};
	Texture mArrowTexture{};
	Texture mAliceTexture{};

	std::array<Texture, 3> mArchersIdleTextures{};
	std::array<Texture, 3> mArchersRunTextures{};
	std::array<Texture, 13> mArchersAttackTextures{};

	std::array<Clip, uint32_t(Monster::eState::Count)> mArcherClips{};

	Entity mMainCamera{};
	Entity mPlayer{};
	std::array<Entity, 1> mMonsters{};
	std::array<Entity, 20> mArrows{};

	// 충돌 관련
private:
	bool isCollisionEnter(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionStay(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionExit(const Entity& entity0, const Entity& entity1) const;

	std::pair<const Entity*, const Entity*> getCollidedEntityPair(const Entity& entity0, const Entity& entity1) const;
	void registerCollidedEntityPairs(const Entity& entity0, const Entity& entity1);

	std::array<Point, 5> convertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const;

	bool checkCollisionBoxBox(const Entity& entity0, const Entity& entity1);

	std::vector<std::pair<const Entity*, const Entity*>> mCollidedEntityPairs{};
	std::vector<std::pair<const Entity*, const Entity*>> mPreviousCollidedEntityPairs{};
};