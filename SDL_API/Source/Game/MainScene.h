#pragma once

#include "Core/Entity/Entity.h"

#include "Core/Clip.h"
#include "Core/ComponentTypes.h"
#include "Core/Font.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

enum class PlayerState
{
	Idle,
	Run,
	Count
};

enum class MonsterState
{
	Idle,
	Run,
	Attack,
	Count
};

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
	Entity mPlayer{};
	Entity mMonster{};

	float mPlayerLength{};
	PlayerState mPlayerState{};
	std::array<Clip, uint32_t(PlayerState::Count)> mPlayerClips{};

	MonsterState mMonsterState{};
	std::array<Clip, uint32_t(MonsterState::Count)> mMonsterClips{};

	static constexpr uint32_t PLAYER_IDLE_COUNT = 5;
	static constexpr uint32_t PLAYER_RUN_COUNT = 6;

	static constexpr uint32_t MONSTER_RUN_COUNT = 8;
	static constexpr uint32_t MONSTER_ATTACK_COUNT = 7;

	std::array<Texture, PLAYER_IDLE_COUNT> mPlayerIdleTextures{};
	std::array<Texture, PLAYER_RUN_COUNT> mPlayerRunTextures{};

	Texture mMonsterIdleTexture{};
	std::array<Texture, MONSTER_RUN_COUNT> mMonsterRunTextures{};
	std::array<Texture, MONSTER_ATTACK_COUNT> mMonsterAttackTextures{};
};