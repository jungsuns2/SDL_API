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
};

struct Sword : public GameComponent
{
	static constexpr uint32_t _ID = 0;
	Sword() : GameComponent(&_ID) {}

	static constexpr uint32_t COUNT = 12;

	Point offset{};
};