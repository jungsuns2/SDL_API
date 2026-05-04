#pragma once
#include "Core/Entity/Component.h"

struct Player : public Component
{
	enum class eState
	{
		Idle,
		Run,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Player() : Component(&_ID) {}

	static constexpr uint32_t IDLE_COUNT = 5;
	static constexpr uint32_t RUN_COUNT = 6;
	eState state{};
	float length{};
	Point direction{};
};

struct Monster : public Component
{
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

	static constexpr uint32_t RUN_COUNT = 8;
	static constexpr uint32_t ATTACK_COUNT = 7;
	static constexpr float SPWAN_POSITION_TIME = 2.0f;
	static constexpr float SPWAN_WAITING_TIME = 1.0f;
	static constexpr float DAMAGE_TIME = 0.3f;
	static constexpr float DEAD_TIME = 1.0f;

	eState state{};
	Point direction{};
	Point difference{};
	float length{};
	float spwanPositionTimer{};
	float spwanWaitingTimer{};
	float spwanBlinkTimer{};
	bool isSpwan = false;
	bool isBlinkOn = false;
	int32_t hp{};
	float damageTimer{};
	float deadTimer{};
};

struct Sword : public Component
{
	static constexpr uint32_t _ID = 0;
	Sword() : Component(&_ID) {}

	static constexpr uint32_t COUNT = 12;

	static constexpr float COOLTIME = 1.0f;
	static constexpr float LENGTH = 200.0f;
	static constexpr float SPEED = 1300.0f;

	Point offset{};
	Point direction{};

	bool isFlying = false;
	float coolTimer{};
};

struct Gun : public Component
{
	static constexpr uint32_t _ID = 0;
	Gun() : Component(&_ID) {}

	Point offset{};
	Point direction{};
};

struct Effect : public Component
{
	static constexpr uint32_t _ID = 0;
	Effect() : Component(&_ID) {}

	static constexpr uint32_t COUNT = 5;
	static constexpr uint32_t BULLET_COUNT = 8;

	Point offset{};
	float directionX{};
};

struct Bullet : public Component
{
	static constexpr uint32_t _ID = 0;
	Bullet() : Component(&_ID) {}

	static constexpr uint32_t COUNT = 8;
	static constexpr float LENGTH = 300.0f;
	static constexpr float COOLTIME = 1.0f;
	static constexpr float SPEED = 1300.0f;

	Point offset{};
	Point direction{};

	bool isFile = false;
	float fireTimer{};
	float coolTimer{};
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
};