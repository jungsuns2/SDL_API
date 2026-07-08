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
};