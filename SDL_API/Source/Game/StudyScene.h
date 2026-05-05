#pragma once

#include "Core/ComponentTypes.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

#include "Core/Entity/Entity.h"

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
	std::pair<const Entity*, const Entity*> getCollidedEntityPair(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionEnter(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionStay(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionExit(const Entity& entity0, const Entity& entity1) const;

private:
	Texture mBoxTexture{};

	Entity mMainCamera{};
	Entity mMobs[2]{};
	std::vector<std::pair<const Entity*, const Entity*>> mCollidedEntityPairs;
	std::vector<std::pair<const Entity*, const Entity*>> mPreviousCollidedEntityPairs;
};