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

	Point getScreenMousePosition() const;
	Point getWeaponPosition(Transform* swordTransform, Transform* playerTransform, float playerRadius);

private:
	bool mIsUpdate = true;
	static constexpr Point PLAYER_HAND = { .x = 15.0f, .y = 30.0f };
	uint32_t mTileWidth{};
	uint32_t mTileHeight{};

	Font mFont{};

	Entity mMainCamera{};
	Entity mLabelEntity{};
	Entity mPlayerEntity{};
	Entity mMonsterEntity{};
	Entity mSwordEntity{};
	Entity mGunEntity{};
	Entity mBulletEntity{};
	Entity** mTileEntities = nullptr;

	Player mPlayer{};
	Monster mMonster{};
	Sword mSword{};
	Gun mGun{};
	Bullet mBullet{};
	Tile mTile[20][20]{};

	std::array<Clip, uint32_t(Player::State::Count)> mPlayerClips{};
	std::array<Clip, uint32_t(Monster::State::Count)> mMonsterClips{};
	Clip mSwordClip{};
	Clip mSwordSkillClip{};
	Clip mBulletClip{};

	std::array<Texture, Player::IDLE_COUNT> mPlayerIdleTextures{};
	std::array<Texture, Player::RUN_COUNT> mPlayerRunTextures{};

	Texture mMonsterSpwanTexture{};
	Texture mMonsterIdleTexture{};
	std::array<Texture, Monster::RUN_COUNT> mMonsterRunTextures{};
	std::array<Texture, Monster::ATTACK_COUNT> mMonsterAttackTextures{};

	std::array<Texture, Sword::COUNT> mSwordTextures{};

	Texture mGunTexture{};
	std::array<Texture, Effect::BULLET_COUNT> mBulletTextures{};

	std::array<Texture, 2> mTileTextures;
};