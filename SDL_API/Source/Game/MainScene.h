#pragma once

#include "Core/Entity/Entity.h"

#include "Core/Clip.h"
#include "Core/ComponentTypes.h"
#include "Core/Font.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

#include "GameComponentTypes.h"

struct BoxCollider;
struct CircleCollider;
struct LineCollider;

struct SetWeaponDesc
{
	Entity* weaponEntity;
	const float playerRadius;
	const float dgreeOffset;
	const SDL_RendererFlip flipX;
	const SDL_RendererFlip flipY;
};

struct MonsterSpwanDesc
{
	std::vector<Entity>* entities;
	const float spwanPositionTime;
	const RangeX rangeX;
	const RangeY rangeY;
	const float spwanScale;
	const uint32_t maxHp;
	const float deltaTime;
};

struct MonsterStateDesc
{
	std::vector<Entity>* entities;
	const float spwanPositionTime;
	const float spwanScale;
	const float originScale;
	const float spwanWaitingTime;
	const float attackDistance;
	const float deltaTime;
};

class EntityWorld;

class MainScene final : public Scene
{
	enum class CollisionLayer
	{
		Player,
		Monster,
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

public:
	void initialize_Resource();
	void initialize_Entity();

	void input();
	void playerState();
	void playerMove(const float deltaTime);
	void playerSetClip();

	void monsterSpwan(const MonsterSpwanDesc& desc);
	void monsterState(const MonsterStateDesc& desc);
	void monsterDeadParticle(std::vector<Entity>* entities, const float deadTime, const float speed, const float deltaTime);
	void monsterMove(std::vector<Entity>* entities, const float maxSpeed, const float deltaTime);
	void monsterSetClip(std::vector<Entity>* entities, std::array<Clip, uint32_t(Monster::eState::Count)>& clips);

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
	Entity mPlayer{};
	std::vector<Entity> mMonsters{};
	Entity mSword{};
	Entity mGun{};
	Entity mBullet{};
	Entity** mTiles = nullptr;
	std::array<Entity, 6> mDeadParticle{};

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