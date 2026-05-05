#pragma once

#include "Core/ComponentTypes.h"
#include "Core/Scene.h"

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
	Entity mMainCamera{};
	Entity mMobs[128]{};
	std::vector<std::pair<const Entity*, const Entity*>> mCollidedEntityPairs;
	std::vector<std::pair<const Entity*, const Entity*>> mPreviousCollidedEntityPairs;
};