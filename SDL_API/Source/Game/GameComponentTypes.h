#pragma once
#include "GameComponent.h"

struct Player : public GameComponent
{
	enum class State
	{
		Idle,
		Run,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Player() : GameComponent(&_ID) {}

	static constexpr uint32_t IDLE_COUNT = 5;
	static constexpr uint32_t RUN_COUNT = 6;
	State state{};
	float length{};
	Point direction{};
};

struct Monster : public GameComponent
{
	enum class State
	{
		Spwan,
		Run,
		Attack,
		Dead,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Monster() : GameComponent(&_ID) {}

	static constexpr uint32_t RUN_COUNT = 8;
	static constexpr uint32_t ATTACK_COUNT = 7;
	static constexpr float SPWAN_POSITION_TIME = 2.0f;
	static constexpr float SPWAN_WAITING_TIME = 1.0f;
	static constexpr float RUN_TIME = 2.0f;
	State state{};
	Point direction{};
	Point difference{};
	float length{};
	float spwanPositionTimer{};
	float spwanWaitingTimer{};
	float spwanBlinkTimer{};
	bool isSpwan = false;
	bool isBlinkOn = false;
};

struct Sword : public GameComponent
{
	static constexpr uint32_t _ID = 0;
	Sword() : GameComponent(&_ID) {}

	static constexpr uint32_t COUNT = 12;

	static constexpr float COOLTIME = 1.0f;
	static constexpr float LENGTH = 200.0f;
	static constexpr float SPEED = 1300.0f;

	Point offset{};
	Point direction{};

	bool isFlying = false;
	float coolTimer{};
};

struct Gun : public GameComponent
{
	static constexpr uint32_t _ID = 0;
	Gun() : GameComponent(&_ID) {}

	Point offset{};
	Point direction{};
};

struct Effect : public GameComponent
{
	static constexpr uint32_t _ID = 0;
	Effect() : GameComponent(&_ID) {}

	static constexpr uint32_t COUNT = 5;
	static constexpr uint32_t BULLET_COUNT = 8;

	Point offset{};
	float directionX{};
};

struct Bullet : public GameComponent
{
	static constexpr uint32_t _ID = 0;
	Bullet() : GameComponent(&_ID) {}

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

struct Tile : public GameComponent
{
	enum class Type
	{
		Green,
		BB,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Tile() : GameComponent(&_ID) {}

	Type type{};
};