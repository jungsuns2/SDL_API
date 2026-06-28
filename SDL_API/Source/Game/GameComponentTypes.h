#pragma once
#include "Core/Entity/Component.h"
#include "Core/Entity/Entity.h"

#include "MonsterGroupDescs.h"

class Clip;

struct PlayerTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerTag() : Component(&_ID) {}
};

struct LeftHandTag : public Component
{
	static constexpr uint32_t _ID = 0;
	LeftHandTag() : Component(&_ID) {}
};

struct RightHandTag : public Component
{
	static constexpr uint32_t _ID = 0;
	RightHandTag() : Component(&_ID) {}
};

struct ShadowTag : public Component
{
	static constexpr uint32_t _ID = 0;
	ShadowTag() : Component(&_ID) {}
};

struct PlayerHpBarTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerHpBarTag() : Component(&_ID) {}
};

struct SwordTag : public Component
{
	static constexpr uint32_t _ID = 0;
	SwordTag() : Component(&_ID) {}
};

struct SwordSkillTag : public Component
{
	static constexpr uint32_t _ID = 0;
	SwordSkillTag() : Component(&_ID) {}
};

struct GunTag : public Component
{
	static constexpr uint32_t _ID = 0;
	GunTag() : Component(&_ID) {}
};

struct BulletTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BulletTag() : Component(&_ID) {}
};

struct BulletCountTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BulletCountTag() : Component(&_ID) {}
};

struct WaveTimerTag : public Component
{
	static constexpr uint32_t _ID = 0;
	WaveTimerTag() : Component(&_ID) {}
};

struct WaveStageTag : public Component
{
	static constexpr uint32_t _ID = 0;
	WaveStageTag() : Component(&_ID) {}
};

struct MonsterTag : public Component
{
	static constexpr uint32_t _ID = 0;
	MonsterTag() : Component(&_ID) {}
};

struct BossTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossTag() : Component(&_ID) {}
};

struct BossBackTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossBackTag() : Component(&_ID) {}
};

struct BossLeftHandTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossLeftHandTag() : Component(&_ID) {}
};

struct BossRightHandTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossRightHandTag() : Component(&_ID) {}
};

struct MonsterHpBarTag : public Component
{
	static constexpr uint32_t _ID = 0;
	MonsterHpBarTag() : Component(&_ID) {}
};

struct MonsterArrowTag : public Component
{
	static constexpr uint32_t _ID = 0;
	MonsterArrowTag() : Component(&_ID) {}
};

struct MonsterRangedAttackTag : public Component
{
	static constexpr uint32_t _ID = 0;
	MonsterRangedAttackTag() : Component(&_ID) {}
};

struct MonsterAttackColliderTag : public Component
{
	static constexpr uint32_t _ID = 0;
	MonsterAttackColliderTag() : Component(&_ID) {}
};

struct CycloneFanTag : public Component
{
	static constexpr uint32_t _ID = 0;
	CycloneFanTag() : Component(&_ID) {}
};

struct Player : public Component
{
	enum class eState
	{
		Idle,
		Run,
		Dead,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Player() : Component(&_ID) {}

	eState state = Player::eState::Idle;
	float length = 0.0f;

	Entity* entity = nullptr;
};

struct Monster : public Component
{
	enum class eState
	{
		Spawn,
		Idle,
		Run,
		Attack,
		Rush,
		Dead,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Monster() : Component(&_ID) {}

	Clip* clips = nullptr;
	eState state{};
	eMonsterType type = eMonsterType::None;
	eAttackType attackType = eAttackType::None;
	bool isRangeAttack = false;
	float speed = 0.0f;
	float length = 0.0f;
	float attackDistance = 0.0f;
	float spawnBlinkTimer = 0.0f;
};

struct Tile : public Component
{
	enum class eType
	{
		Green,
		BB,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Tile() : Component(&_ID) {}

	eType type{};
	uint32_t maxCount{};
};

struct Particle : public Component
{
	static constexpr uint32_t _ID = 0;
	Particle() : Component(&_ID) {}
};

struct DamageTimer : public Component
{
	static constexpr uint32_t _ID = 0;
	DamageTimer() : Component(&_ID) {}

	float damageTimer{};
	float deadTimer{};
};

struct Hp : public Component
{
	static constexpr uint32_t _ID = 0;
	Hp() : Component(&_ID) {}

	uint32_t max = 0;
	int32_t value = 0;
	uint32_t hpBarIndex = 0;
};

struct Knockback : public Component
{
	static constexpr uint32_t _ID = 0;
	Knockback() : Component(&_ID) {}

	bool isValue = false;
	float coolTimer{};
	Point direction{};
};

struct RangedAttack : public Component
{
	static constexpr uint32_t _ID = 0;
	RangedAttack() : Component(&_ID) {}

	float distance = 0.0f;
	bool isFiring = false;
	Point startPosition = { .x = 0.0f, .y = 0.0f };
};

struct AttackPattern : public Component
{
	enum class eType
	{
		None,
		Rush,
		CycloneFan,
		Hands,
		Count
	};

	static constexpr uint32_t _ID = 0;
	AttackPattern() : Component(&_ID) {}

	eType type = eType::None;
	float timer = 0.0f;
	float distanceMoved = 0.0f;
};

struct ColliderState : public Component
{
	static constexpr uint32_t _ID = 0;
	ColliderState() : Component(&_ID) {}

	Scale runSize = { .width = 0.0f, .height = 0.0f };
	Point runOffset = { .x = 0.0f, .y = 0.0f };
	
	Scale attackSize = { .width = 0.0f, .height = 0.0f };
	Point attackOffset = { .x = 0.0f, .y = 0.0f };

	uint32_t attackAnimIndex = 0;
};

struct MonsterAttackCollider : public Component
{
	static constexpr uint32_t _ID = 0;
	MonsterAttackCollider() : Component(&_ID) {}

	Entity* ownerEntity = nullptr;
};

struct Damage : public Component
{
	static constexpr uint32_t _ID = 0;
	Damage() : Component(&_ID) {}

	uint32_t value = 0;
};

struct Dash : public Component
{
	static constexpr uint32_t _ID = 0;
	Dash() : Component(&_ID) {}

	uint32_t count = 0;
	uint32_t maxCount = 0;
	float moveSpeed = 0.0f;
	bool isValue = false;
	bool isShadow = false;
	Point startPosition = { .x = 0.0f, .y = 0.0f };
	Point direction = { .x = 0.0f, .y = 0.0f };
	float countTimer = 0.0f;
	float shadowTimer = 0.0f;
};

struct Shadow : public Component
{
	static constexpr uint32_t _ID = 0;
	Shadow() : Component(&_ID) {}

	float timer = 0.0f;
};