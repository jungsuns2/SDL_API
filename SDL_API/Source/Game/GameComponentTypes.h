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

struct PlayerHpBarBackGroundTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerHpBarBackGroundTag() : Component(&_ID) {}
};

struct PlayerHpBarBolderTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerHpBarBolderTag() : Component(&_ID) {}
};

struct PlayerIconTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerIconTag() : Component(&_ID) {}
};

struct PlayerHpBarTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerHpBarTag() : Component(&_ID) {}
};

struct PlayerHpLabelTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerHpLabelTag() : Component(&_ID) {}
};

struct PlayerHpBarAllTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerHpBarAllTag() : Component(&_ID) {}
};

struct PlayerDashBackGroundTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerDashBackGroundTag() : Component(&_ID) {}
};

struct PlayerDashUiTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerDashUiTag() : Component(&_ID) {}
};

struct PlayerDashUiAllTag : public Component
{
	static constexpr uint32_t _ID = 0;
	PlayerDashUiAllTag() : Component(&_ID) {}
};

struct SwordTag : public Component
{
	static constexpr uint32_t _ID = 0;
	SwordTag() : Component(&_ID) {}
};

struct SwordAttackTag : public Component
{
	static constexpr uint32_t _ID = 0;
	SwordAttackTag() : Component(&_ID) {}
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

struct NormalMonsterTag : public Component
{
	static constexpr uint32_t _ID = 0;
	NormalMonsterTag() : Component(&_ID) {}
};

struct BossBackTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossBackTag() : Component(&_ID) {}
};

struct BossTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossTag() : Component(&_ID) {}
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

struct BossHandSkillTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossHandSkillTag() : Component(&_ID) {}
};

struct BossHandCenterSkillTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossHandCenterSkillTag() : Component(&_ID) {}
};

struct BossHpBarBackGroundTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossHpBarBackGroundTag() : Component(&_ID) {}
};

struct BossHpBarBolderTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossHpBarBolderTag() : Component(&_ID) {}
};

struct BossIconTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossIconTag() : Component(&_ID) {}
};

struct BossHpBarTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossHpBarTag() : Component(&_ID) {}
};

struct BossHpLabelTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossHpLabelTag() : Component(&_ID) {}
};

struct BossHpBarAllTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BossHpBarAllTag() : Component(&_ID) {}
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

struct BigWhiteHitboxTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BigWhiteHitboxTag() : Component(&_ID) {}
};

struct CycloneFanTag : public Component
{
	static constexpr uint32_t _ID = 0;
	CycloneFanTag() : Component(&_ID) {}
};

struct NextBackGroundTag : public Component
{
	static constexpr uint32_t _ID = 0;
	NextBackGroundTag() : Component(&_ID) {}
};

struct EndingBackGroundTag : public Component
{
	static constexpr uint32_t _ID = 0;
	EndingBackGroundTag() : Component(&_ID) {}
};

struct LoseLabelTag : public Component
{
	static constexpr uint32_t _ID = 0;
	LoseLabelTag() : Component(&_ID) {}
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

struct HitboxSponwer : public Component
{
	static constexpr uint32_t _ID = 0;
	HitboxSponwer() : Component(&_ID) {}

	bool isSpawned = false;
	Entity* hitboxEntity = nullptr;
};

struct BossHand : public Component
{
	enum class eState
	{
		Idle,
		Attack,
		Count
	};

	static constexpr uint32_t _ID = 0;
	BossHand() : Component(&_ID) {}

	eState state{};
	Clip* clips = nullptr;
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
	Point startPosition = { .x = 0.0f, .y = 0.0f };
};

struct AttackPattern : public Component
{
	enum class eType
	{
		None,
		Rush,
		CycloneFan,
		HandSkill,
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

struct Hitbox : public Component
{
	static constexpr uint32_t _ID = 0;
	Hitbox() : Component(&_ID) {}

	Entity* targetEntity = nullptr;
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