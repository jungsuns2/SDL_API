#pragma once

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

constexpr std::array<MonsterGroup, 13> MONSTER_GROUPS =
{
	// 0
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 1,
		.rangeX = { .min = -100.0f, .max = 100.0f },
		.rangeY = { .min = -100.0f, .max = 100.0f }
	}, 

	// 1
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 2,
		.rangeX = { .min = -100.0f, .max = 100.0f },
		.rangeY = { .min = -100.0f, .max = 100.0f }
	}, 

	// 2
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 3, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }

	}, 

	// 3
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 4, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }

	}, 

	// 4
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 1,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	},

	// 5
		MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 2,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	},

	// 6
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 3, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 7
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 4, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 8
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.count = 1,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 9
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.count = 2,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 10
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.count = 3,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 
	
	// 11
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.count = 4,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
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