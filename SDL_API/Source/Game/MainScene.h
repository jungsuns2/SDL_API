#pragma once

#include "Core/Entity/Entity.h"

#include "Core/Clip.h"
#include "Core/ComponentTypes.h"
#include "Core/Font.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

#include "GameComponentTypes.h"

class EntityWorld;

class MainScene final : public Scene
{
public:
	MainScene() = default;
	MainScene(const MainScene&) = delete;
	const MainScene& operator=(const MainScene&) = delete;
	~MainScene() = default;

public:
	void Initialize() override;
	bool Update(const float deltaTime) override;
	void Finalize()  override;

public:
	void Input();
	void State();
	void Move(const float deltaTime);
	void SetClip();

private:
	bool mIsUpdate = true;

	Font mFont{};

	Entity mMainCamera{};
	Entity mLabel{};
	Entity mPlayerEntity{};
	Entity mMonsterEntity{};

	Player mPlayer{};
	Monster mMonster{};

	std::array<Clip, uint32_t(Player::State::Count)> mPlayerClips{};
	std::array<Clip, uint32_t(Monster::State::Count)> mMonsterClips{};

	std::array<Texture, Player::IDLE_COUNT> mPlayerIdleTextures{};
	std::array<Texture, Player::RUN_COUNT> mPlayerRunTextures{};
	Texture mMonsterIdleTexture{};
	std::array<Texture, Monster::RUN_COUNT> mMonsterRunTextures{};
	std::array<Texture, Monster::ATTACK_COUNT> mMonsterAttackTextures{};
};