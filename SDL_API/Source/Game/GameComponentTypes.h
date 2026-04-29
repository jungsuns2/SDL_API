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
		Idle,
		Run,
		Attack,
		Count
	};

	static constexpr uint32_t _ID = 0;
	Monster() : GameComponent(&_ID) {}

	static constexpr uint32_t RUN_COUNT = 8;
	static constexpr uint32_t ATTACK_COUNT = 7;
	State state{};
	Point direction{};
	Point difference{};
	float length{};
};

struct Sword : public GameComponent
{
	static constexpr uint32_t _ID = 0;
	Sword() : GameComponent(&_ID) {}

	static constexpr uint32_t COUNT = 12;

	static constexpr float ANGLE = 60.0f;

	Point offset{};
	float directionX{};

	float coolTimer{};
	bool isSwinging = false;
	float swingTime{};
	float coolTime{};
};

struct Gun : public GameComponent
{
	static constexpr uint32_t _ID = 0;
	Gun() : GameComponent(&_ID) {}

	Point offset{};
	float directionX{};
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
	static constexpr float COOLTIMER = 1.0f;
	static constexpr float SPEED = 1300.0f;

	Point offset{};
	Point direction{};

	bool isFile = false;
	float fireTime{};
	float coolTime{};

};