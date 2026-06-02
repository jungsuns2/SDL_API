#pragma once
#include "Core/Entity/Component.h"

#include "MonsterGroupDescs.h"

class Clip;

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

	eState state{};
	float length{};
};

struct Monster : public Component
{
	enum class eState
	{
		Spawn,
		Idle,
		Run,
		Attack,
		Dead,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Monster() : Component(&_ID) {}

	Clip* clips = nullptr;
	eState state{};
	eMonsterType type = eMonsterType::None;
	float speed = 0.0f;
	bool isAttackOption = false;
	float length = 0.0f;
	float attackDistance = 0.0f;
	float spawnBlinkTimer = 0.0f;
};

struct Sword : public Component
{
	static constexpr uint32_t _ID = 0;
	Sword() : Component(&_ID) {}
};

struct Gun : public Component
{
	static constexpr uint32_t _ID = 0;
	Gun() : Component(&_ID) {}
};

struct Bullet : public Component
{
	static constexpr uint32_t _ID = 0;
	Bullet() : Component(&_ID) {}
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

struct WeaponState : public Component
{
	static constexpr uint32_t _ID = 0;
	WeaponState() : Component(&_ID) {}

	bool isFire = false;  
	float fireCoolTimer{};
};

struct Knockback : public Component
{
	static constexpr uint32_t _ID = 0;
	Knockback() : Component(&_ID) {}

	bool isValue = false;
	float coolTimer{};
	Point direction{};
};

struct Effect : public Component
{
	static constexpr uint32_t _ID = 0;
	Effect() : Component(&_ID) {}

	bool isDisabled = false;
	float coolTimer{};
};

struct RangedAttack : public Component
{
	static constexpr uint32_t _ID = 0;
	RangedAttack() : Component(&_ID) {}

	float distance = 0.0f;
	bool isFire = false;
	Point startPosition = { .x = 0.0f, .y = 0.0f };
};

struct ColliderState : public Component
{
	static constexpr uint32_t _ID = 0;
	ColliderState() : Component(&_ID) {}

	Scale idleSize = { .width = 0.0f, .height = 0.0f };
	Scale runSize = { .width = 0.0f, .height = 0.0f };
	Scale attackSize = { .width = 0.0f, .height = 0.0f };

	Point idleOffset = { .x = 0.0f, .y = 0.0f };
	Point runOffset = { .x = 0.0f, .y = 0.0f };
	Point attackOffset = { .x = 0.0f, .y = 0.0f };
};