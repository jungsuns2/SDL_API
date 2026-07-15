#pragma once
#include "Core/Constant.h"

enum class eMonsterType
{
	None,
	BigWhite,
	Archer,
	SkelDog,
	Boss,
	Count
};

enum class eAttackType
{
	None,
	Melee,
	Range,
	Count
};

struct MonsterGroup
{
	eMonsterType type;
	uint32_t count;
	Range rangeX;
	Range rangeY;
};

constexpr Point randomPosition = { .x = 400.0f, .y = 300.0f };

constexpr std::array<MonsterGroup, 13> MONSTER_GROUPS =
{
	// 0
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 1,
		.rangeX = { .min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = { .min = -randomPosition.y, .max = randomPosition.y }
	}, 

	// 1
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 2,
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }
	},

	// 2
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 3, 
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }

	}, 

	// 3
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 4, 
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }

	}, 

	// 4
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 1,
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }
	},

	// 5
		MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 2,
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }
	},

	// 6
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 3, 
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }
	},

	// 7
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 4, 
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }
	},

	// 8
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.count = 1,
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }
	},

	// 9
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.count = 2,
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }
	},

	// 10
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.count = 3,
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }
	},
	
	// 11
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.count = 4,
		.rangeX = {.min = -randomPosition.x, .max = randomPosition.x },
		.rangeY = {.min = -randomPosition.y, .max = randomPosition.y }
	},
	
	// 12
	MonsterGroup
	{
		.type = eMonsterType::Boss,
		.count = 1,
		.rangeX = {.min = 0.0f, .max = 0.0f },
		.rangeY = {.min = 0.0f, .max = 0.0f }
	}, 
};