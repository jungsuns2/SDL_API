#pragma once

#include "Core/Entity/Entity.h"
#include "Core/Entity/EntityWorld.h"

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

struct SpawnMonsterDesc
{
	const uint32_t index;
	const eMonsterType type;
	const float x;
	const float y;
};

struct GameWaveState
{
	uint32_t index;
	uint32_t groupIndex;

	float waveTimer; // 현재 웨이브에서 흐른 시간
	float remainingMonsterGroupSpawnTimer;
	float labelShowElapsedTimer;
};

struct BulletState
{
	uint32_t maxCount;
	uint32_t count;
	float fireTimer;
	float reloadTimer;
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
		SwordSkill,
		Bullet,
		Arrow,
		CycloneFan,
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
	void updateCamera();
	[[nodiscard]] Rect& getCameraRect() const;
		
	template <typename T>
	Entity* getEntity() const
	{
		for (Entity* entity : GetEntityWorld()->GetAllEntities())
		{
			if (not entity->HasComponent<T>())
			{
				continue;
			}

			return entity;
		}

		return nullptr;
	}
		
	template <typename T>
	std::vector<Entity*> getEntities() const
	{
		std::vector<Entity*> result{};

		for (Entity* entity : GetEntityWorld()->GetAllEntities())
		{
			if (not entity->HasComponent<T>())
			{
				continue;
			}

			result.push_back(entity);
		}

		return result;
	}

	void initializeUI();
	void initializeWaveTimer();
	void initializeWaveStage();

	void initializePlayer();
	void playerState(const float deltaTime);
	void playerMove(const float deltaTime);
	void playerSetClip();

	void initializeGun();
	void updateGun();

	void initializeSword();
	void updateSword();
	void updateSwordStates(const float deltaTime);

	void initializeSwordSkill();
	void spawnSwordSkill();
	void updateSwordSkill(const float deltaTime);
	void updateSwordSkillStates(const float deltaTime);

	void initializeBullets();
	void spawnBullets(const float deltaTime);
	void updateBullets(const float deltaTime);
	void updateBulletStates(const float deltaTime);

	void spawnMonsterGroup(const MonsterGroup& group);
	void spawnMonster(const SpawnMonsterDesc& desc);
	void updateMonsterStates(const float deltaTime);
	void updateHpMonsters(const float deltaTime);
	void monsterMove(const float deltaTime);
	void monsterHpBarMove();
	void setDirectionOffset(Entity* setEntity, const Entity& entity0);
	void monsterSetClip();

	void initializeBossBack();
	void initializeBoss();
	void spawnBoss();
	void updateBossStates(const float deltaTime);
	void BossSetClip();

	void initializeCycloneFan();
	void spawnCycloneFan(const float deltaTime);
	void updateCycloneFan(const float deltaTime);

	void initializeBossHands();

	void spawnAttackCollider();
	void updateAttackCollision();
	void removeAttackCollider();

	void playerToMonsterCollision();
	void playerToMonsterAttackCollision();
	void playerToArrowCollision();
	void swordSkillToMonsterCollision();
	void bulletToMonsterCollision();

	void spawnRangedAttack(const eMonsterType type, const uint32_t spawnFrameIndex);
	void rangedAttackState();
	void rangedAttackMove(const float speed, const float deltaTime);

	void clampToTile(Transform* transform, const Range rangeX, const Range RangeY);

	Point getScreenMousePosition() const;

	float getRandom(const float min, const float max);

private:
	bool mIsUpdate = true;
	bool mIsDebugActive = false;

	uint32_t mTileMaxCount{};
	float mTilePositionOffset{};

	BulletState mBulletState{};
	BulletState mCycloneFanState{};

	GameWaveState mGameWaveState{};

	uint32_t mMonsterIndex = 0;

	Entity** mTiles = nullptr;

	Entity mBoss{};
	Entity mBossHpBar{};
	Entity mBossLeftHand{};
	Entity mBossRightHand{};
	Entity mBossBack{};
	std::array<Entity, 10> mCycloneFans{};
	
	Font mUIFont{};
	Font mHpFont{};

	std::array<Clip, uint32_t(Player::eState::Count)> mPlayerClips{};

	std::array<Clip, uint32_t(Monster::eState::Count)> mBigWhiteSkelClips{};
	std::array<Clip, uint32_t(Monster::eState::Count)> mArcherClips{};
	std::array<Clip, uint32_t(Monster::eState::Count)> mSkelDogClips{};
	std::array<Clip, uint32_t(Monster::eState::Count)> mBossClips{};
	Clip mBossLeftHandClip{};
	Clip mBossRightHandClip{};
	Clip mBossBackClip{};
	Clip mCycloneFanClip{};

	Clip mSwordClip{};
	Clip mSwordSkillClip{};
	Clip mBulletClip{};

	std::array<Texture, 5> mPlayerIdleTextures{};
	std::array<Texture, 6> mPlayerRunTextures{};
	Texture mPlayerShadowTexture{};
	Texture mPlayerHandTexture{};
	Texture mPlayerDeadTexture{};

	std::array<Texture, 2> mSpwanTextures{};

	std::array<Texture, 6> mBigWhiteSkelRunTextures{};
	std::array<Texture, 12> mBigWhiteSkelAttackTextures{};

	Texture mArrowTexture{};
	std::array<Texture, 3> mArcherRunTextures{};
	std::array<Texture, 13> mArcherAttackTextures{};

	std::array<Texture, 5> mSkelDogIdleTextures{};
	std::array<Texture, 7> mSkelDogRunTextures{};
	Texture mSkelDogAttackTexture{};

	std::array<Texture, 10> mBossIdleTextures{};
	std::array<Texture, 12> mBossAttackTextures{};
	std::array<Texture, 10> mBossLeftHandTextures{};
	std::array<Texture, 10> mBossBackTextures{};
	std::array<Texture, 2> mCycloneFanTextures{};

	std::array<Texture, 12> mSwordTextures{};
	std::array<Texture, 8> mSwordSkillTextures{};

	Texture mGunTexture{};
	std::array<Texture, 8> mBulletTextures{};

	std::array<Texture, 2> mTileTextures{};

	Texture mRedRectTexture{};

private:
	bool isCollisionEnter(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionStay(const Entity& entity0, const Entity& entity1) const;
	bool isCollisionExit(const Entity& entity0, const Entity& entity1) const;

	std::pair<const Entity*, const Entity*> getCollidedEntityPair(const Entity& entity0, const Entity& entity1) const;
	void registerCollidedEntityPairs(const Entity& entity0, const Entity& entity1);

	Quad convertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const;
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