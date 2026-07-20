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

struct SpawnRangeAttackDesc
{
	const eMonsterType type;
	Texture* texture;
	const uint32_t spawnFrameIndex;
};

struct GameWaveState
{
	uint32_t index;
	uint32_t groupIndex;

	float waveTimer; // 현재 웨이브에서 흐른 시간
	float remainingMonsterGroupSpawnTimer;
	float labelShowElapsedTimer;
};

struct SwordAttackState
{
	bool isSpawn;
	float coolTimer;
};

struct BulletState
{
	uint32_t maxCount;
	uint32_t count;
	bool isfire;
	float fireTimer;
	bool isReload;
	float reloadTimer;
};

struct CycloneFanState
{
	uint32_t maxCount;
	uint32_t count;
	float fireTimer;
	float reloadTimer;
	bool isUpdate;
};

struct HandSkillState
{
	Entity* entity;
	bool isSpawn;
	bool isAttack;
	float attackTimer;
};

struct PlayerWeaponState
{
	bool isSword;
};

class EntityWorld;

class MainScene final : public Scene
{
public:
	enum class CollisionLayer
	{
		None,

		Monster,
		MonsterAttack,
		MonsterArrow,

		CycloneFan,
		HandSkill,

		Player,
		SwordAttack,
		PlayerBullet,

		Count
	};

	enum class Layer
	{
		None,
		Tile,
		BossBack,
		Monster,
		Shadow,
		Player,
		UI,
		BackGround,
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

	void input();
	void updateCamera(Entity* targetEntity);
	[[nodiscard]] Rect getCameraRect() const;
		
	template <typename T>
	Entity* getEntity() const;

	template <typename T>
	std::vector<Entity*> getEntities() const;

	void initializeTile();
	
	void initializePlayer();

	void initializeSword();
	void initializeGun();

	void initializePlayerDashUi();
	void initializePlayerHpBarUi();
	void initializePlayerBulletUi();

	void initializeWaveTimer();
	void initializeWaveStage();

	void initializeEnding(Uint8 opaque = 255);
	void initializeEndingLabel(const std::string& text);
	void updateEnding(const float deltaTime);

	void playerDash(const Point& moveDirection, const float deltaTime);
	void playerSpawnShadow(const float deltaTime);
	void playerUpdateShadow(const float deltaTime);
	void playerState(const float deltaTime);
	void playerMove(const float deltaTime);
	void playerSetClip();

	void changePlayerWeapon();

	void updateGun();

	void updateSword();
	void updateSwordStates(const float deltaTime);

	void spawnSwordAttack();
	void updateSwordAttack(const float deltaTime);
	void updateSwordAttackStates(const float deltaTime);

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

	void updateBossStates(const float deltaTime);
	void bossSetClip();
	void bossHandsSetClip();

	void spawnWingBullet(const float wingOffsetAngle, const uint32_t index);
	void spawnCycloneFan(const float deltaTime);
	void updateCycloneFan(const float deltaTime);
	
	void spawnHandSkill();
	void updateHandSkill();
	void handSkillSetClip();

	void spawnHitbox();
	void updateHitbox(const float deltaTime);

	void gameReset();

	void playerToMonsterCollision();
	void playerToMonsterHitboxCollision();
	void playerToArrowCollision();
	void swordAttackToMonsterCollision();
	void bulletToMonsterCollision();

	void playerToBossCollision();
	void swordAttackToBossCollision(const float deltaTime);
	void bulletToBossCollision();
	void playerToBossHitboxCollision();
	void playerToBossHandAttackCollision();

	template<typename T>
	void spawnRangedAttack(const SpawnRangeAttackDesc& desc);

	template<typename T>
	void rangedAttackState();

	void rangedAttackMove(const float speed, const float deltaTime);

	void clampToTile(Transform* transform, const Range rangeX, const Range RangeY);

	Point getScreenMousePosition() const;

	float getRandom(const float min, const float max);
	uint32_t getRandom(const uint32_t min, const uint32_t max);

private:
	bool mIsMainSceneUpdate = true;
	bool mIsGameUpdate = false;
	bool mIsDebugActive = false;

	uint32_t mTileMaxCount = 0;
	float mTilePositionOffset = 0.0f;

	GameWaveState mGameWaveState{};
	
	SwordAttackState mSwordAttackState{};
	BulletState mBulletState{};

	CycloneFanState mCycloneFanState{};
	HandSkillState mHandSkillState{};

	PlayerWeaponState mPlayerWeaponState{};

	uint32_t mMonsterIndex = 0;
	int32_t mDashShadowOffsetIndex = -1;
	Entity** mTiles = nullptr;
	
	Font mUIFont{};
	Font mHpFont{};

	std::array<Clip, uint32_t(Player::eState::Count)> mPlayerClips{};

	std::array<Clip, uint32_t(Monster::eState::Count)> mBigWhiteSkelClips{};
	std::array<Clip, uint32_t(Monster::eState::Count)> mArcherClips{};
	std::array<Clip, uint32_t(Monster::eState::Count)> mSkelDogClips{};
	std::array<Clip, uint32_t(Monster::eState::Count)> mBossClips{};
	Clip mBossBackClip{};
	Clip mCycloneFanClip{};
	std::array<Clip, uint32_t(BossHand::eState::Count)> mBossHandClips{};
	Clip mBossHandStartSkillClip{};
	Clip mBossHandCenterSkillClip{};

	Clip mSwordClip{};
	Clip mSwordAttackClip{};
	Clip mBulletClip{};

	Texture mRedRectTexture{};
	
	Texture mBgSkyNightTexture{};

	std::array<Texture, 2> mTileTextures{};
	Texture mArrowTexture{};

	Texture mPlayerHpBarBackGroundTexture{};
	Texture mPlayerHpBarBorderTexture{};
	Texture mPlayerIconTexture{};

	Texture mPlayerDashLeftBackGroundTexture{};
	Texture mPlayerDashCenterBackGroundTexture{};
	Texture mPlayerDashRightBackGroundTexture{};

	Texture mPlayerDashTexture{};

	std::array<Texture, 5> mPlayerIdleTextures{};
	std::array<Texture, 6> mPlayerRunTextures{};
	Texture mPlayerShadowTexture{};
	Texture mPlayerHandTexture{};
	Texture mPlayerDeadTexture{};

	std::array<Texture, 12> mSwordTextures{};
	std::array<Texture, 8> mSwordAttackTextures{};

	Texture mGunTexture{};
	std::array<Texture, 8> mBulletTextures{};

	std::array<Texture, 2> mSpwanTextures{};

	std::array<Texture, 6> mBigWhiteSkelRunTextures{};
	std::array<Texture, 12> mBigWhiteSkelAttackTextures{};

	std::array<Texture, 3> mArcherRunTextures{};
	std::array<Texture, 13> mArcherAttackTextures{};

	std::array<Texture, 5> mSkelDogIdleTextures{};
	std::array<Texture, 7> mSkelDogRunTextures{};
	Texture mSkelDogAttackTexture{};

	Texture mBossHpbarBackGroundTexture{};
	Texture mBossHpBarBorderTexture{};
	Texture mBossIconTexture{};

	std::array<Texture, 10> mBossIdleTextures{};
	std::array<Texture, 12> mBossAttackTextures{};
	std::array<Texture, 6> mBossDeadTextures{};
	std::array<Texture, 10> mBossHandTextures{};
	std::array<Texture, 10> mBossBackTextures{};
	std::array<Texture, 2> mCycloneFanTextures{};
	std::array<Texture, 18> mBossAttackHandTextures{};
	std::array<Texture, 7> mBossHandCenterSkillTextures{};
	std::array<Texture, 7> mBossHandSkillTextures{};
};