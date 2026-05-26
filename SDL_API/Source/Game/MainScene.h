#pragma once

#include "Core/Entity/Entity.h"

#include "Core/Clip.h"
#include "Core/ComponentTypes.h"
#include "Core/Font.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

#include "GameComponentTypes.h"
#include "MonsterGroupDescs.h"
#include "WaveDesc.h"

struct BoxCollider;
struct CircleCollider;
struct LineCollider;

struct SetWeaponDesc
{
	Entity* weaponEntity;
	const float dgreeOffset;
	const SDL_RendererFlip flipX;
	const SDL_RendererFlip flipY;
};

struct GameWaveState
{
	uint32_t index;
	uint32_t groupIndex;

	float waveTimer; // 현재 웨이브에서 흐른 시간.
	float remainingMonsterGroupSpawnTime;
	float labelShowElapsedTime;
};

class EntityWorld;

class MainScene final : public Scene
{
public:
	enum class CollisionLayer
	{
		None,
		Player,
		Monster,
		Arrow,
		Wall,
		Count
	};

public:
	MainScene() = default;
	MainScene(const MainScene&) = delete;
	const MainScene& operator=(const MainScene&) = delete;
	~MainScene() = default;

public:
	void Initialize() override;
	bool Update(const float deltaTime) override;
	void Finalize()  override;

private:
	void initialize_Resource();
	void initialize_Entity();

	void input();
	void playerState(const float deltaTime);
	void playerMove(const float deltaTime);
	void playerSetClip();

	void initializeMonsters();
	void spawnMonsterGroup(const MonsterGroup& group);
	void spawnMonster(Entity* entity, const Monster::eType type, const float x, const float y);
	void updateMonsterStates(const float deltaTime);
	void monsterDeadParticle(const float deltaTime);
	void monsterMove(const float maxSpeed, const float deltaTime);
	void monsterSetClip();

	void clampToTile(Transform* transform, const Range rangeX, const Range RangeY);

	Point getScreenMousePosition() const;
	void setWeaponPosition(const SetWeaponDesc& desc);

	float getRandom(const float min, const float max);

private:
	bool mIsUpdate = true;
	uint32_t mTileMaxCount{};
	float mTilePositionOffset{};

	GameWaveState mGameWaveState{};

	Font mUIFont{};
	Font mHpFont{};

	Entity mMainCamera{};
	Entity mWaveTimerLebel{};
	Entity mStageLabel{};
	Entity mPlayer{};
	Entity mUIPlayerHp{};
	Entity mPlayerLeftHand{};
	Entity mPlayerRightHand{};
	Entity mSword{};
	Entity mSwordSkill{};
	Entity mGun{};
	Entity mBullet{};
	Entity** mTiles = nullptr;
	std::array<Entity, 6> mDeadParticle{};

	std::array<Entity, 20> mMonsters{};

	std::array<Clip, uint32_t(Player::eState::Count)> mPlayerClips{};
	std::array<Clip, uint32_t(Monster::eState::Count)> mBigWhiteSkelClips{};
	std::array<Clip, uint32_t(Monster::eState::Count)> mArcherClips{};

	Clip mSwordClip{};
	Clip mSwordSkillClip{};
	Clip mBulletClip{};

	std::array<Texture, 5> mPlayerIdleTextures{};
	std::array<Texture, 6> mPlayerRunTextures{};
	Texture mPlayerHandTexture{};
	Texture mPlayerDeadTexture{};

	std::array<Texture, 2> mSpwanTextures{};
	std::array<Texture, 5> mBigWhiteSkelIdleTextures{};
	std::array<Texture, 6> mBigWhiteSkelRunTextures{};
	std::array<Texture, 12> mBigWhiteSkelAttackTextures{};

	Texture mArrowTexture{};
	std::array<Texture, 3> mArcherIdleTextures{};
	std::array<Texture, 3> mArcherRunTextures{};
	std::array<Texture, 13> mArcherAttackTextures{};

	std::array<Texture, 12> mSwordTextures{};
	std::array<Texture, 8> mSwordSkillTextures{};

	Texture mGunTexture{};
	std::array<Texture, 8> mBulletTextures{};

	std::array<Texture, 2> mTileTextures{};

	Texture mDeadParticleTexture{};

private:
	bool isCollisionEnter(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionStay(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionExit(const Entity& entity0, const Entity& entity1) const;

	std::pair<const Entity*, const Entity*> getCollidedEntityPair(const Entity& entity0, const Entity& entity1) const;
	void registerCollidedEntityPairs(const Entity& entity0, const Entity& entity1);

	Rect convertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const;
	Circle convertCircleColliderToWorldCircle(const Transform& transform, const CircleCollider& circleCollider) const;
	Line convertLineColliderToWorldLine(const Transform& transform, const LineCollider& lineCollider) const;

	bool checkCollisionBoxBox(const Entity& entity0, const Entity& entity1);
	bool checkCollisionBoxCircle(const Entity& boxEntity, const Entity& circleEntity);
	bool checkCollisionBoxLine(const Entity& boxEntity, const Entity& lineEntity);
	bool checkCollisionCircleCircle(const Entity& entity0, const Entity& entity1);
	bool checkCollisionCircleLine(const Entity& circleEntity, const Entity& lineEntity);

	std::vector<std::pair<const Entity*, const Entity*>> mCollidedEntityPairs{};
	std::vector<std::pair<const Entity*, const Entity*>> mPreviousCollidedEntityPairs{};
};