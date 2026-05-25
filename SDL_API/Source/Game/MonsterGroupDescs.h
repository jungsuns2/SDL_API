#pragma once
#include "GameComponentTypes.h"

struct MonsterGroup
{
	Monster::eType type;
	uint32_t count;
	Range rangeX;
	Range rangeY;
};

constexpr std::array<MonsterGroup, 7> MONSTER_GROUPS =
{
	// 0
	MonsterGroup
	{
		.type = Monster::eType::BigWhite,
		.count = 2,
		.rangeX = { .min = -100.0f, .max = 100.0f },
		.rangeY = { .min = -100.0f, .max = 100.0f }
	}, 

	// 1
	MonsterGroup
	{
		.type = Monster::eType::BigWhite,
		.count = 3, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }

	}, 

	// 2
	MonsterGroup
	{
		.type = Monster::eType::BigWhite,
		.count = 4, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 3
	MonsterGroup
	{
		.type = Monster::eType::Archer,
		.count = 1, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 4
	MonsterGroup
	{
		.type = Monster::eType::Archer,
		.count = 2, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 5
	MonsterGroup
	{
		.type = Monster::eType::Archer,
		.count = 3, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 6
	MonsterGroup
	{
		.type = Monster::eType::Archer,
		.count = 4, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 
};