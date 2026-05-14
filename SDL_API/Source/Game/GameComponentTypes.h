#pragma once
#include "Core/Entity/Component.h"

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
	enum class eType
	{
		Knight,
		Dog,
		Count
	};

	enum class eState
	{
		Spwan,
		Idle,
		Run,
		Attack,
		Dead,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Monster() : Component(&_ID) {}

	eState state{};
	float length{};
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

struct SpwanTimer : public Component
{
	static constexpr uint32_t _ID = 0;
	SpwanTimer() : Component(&_ID) {}

	bool isSpwan = false;
	bool isBlinkOn = false;

	float spwanPositionTimer{};
	float spwanWaitingTimer{};
	float spwanBlinkTimer{};
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

	uint32_t max{};
	int32_t value{};
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

	float coolTimer{};
};