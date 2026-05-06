#pragma once

#include "Core/Entity/Entity.h"

#include "Core/Clip.h"
#include "Core/ComponentTypes.h"
#include "Core/Font.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

#include "GameComponentTypes.h"

struct SetWeaponDesc
{
	Entity* weaponEntity;
	float playerRadius;
	float dgreeOffset;
	SDL_RendererFlip flipX;
	SDL_RendererFlip flipY;
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
	void Initialize_Resource();
	void Initialize_Entity();

	void Input();
	void State();
	void Move(const float deltaTime);
	void SetClip();

	Point getScreenMousePosition() const;
	void setWeaponPosition(const SetWeaponDesc& desc);

	float getRandom(const float min, const float max);

private:
	bool mIsUpdate = true;
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
	std::array<Entity, 6> mDeadParticleEntities{};

	std::array<Clip, uint32_t(Player::eState::Count)> mPlayerClips{};
	std::array<Clip, uint32_t(Monster::eState::Count)> mMonsterClips{};
	Clip mSwordClip{};
	Clip mSwordSkillClip{};
	Clip mBulletClip{};

	std::array<Texture, 5> mPlayerIdleTextures{};
	std::array<Texture, 6> mPlayerRunTextures{};

	Texture mMonsterSpwanTexture{};
	Texture mMonsterIdleTexture{};
	std::array<Texture, 8> mMonsterRunTextures{};
	std::array<Texture, 7> mMonsterAttackTextures{};

	std::array<Texture, 12> mSwordTextures{};

	Texture mGunTexture{};
	std::array<Texture, 8> mBulletTextures{};

	std::array<Texture, 2> mTileTextures{};

	Texture mDeadParticleTexture{};
};