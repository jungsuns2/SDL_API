#pragma once

enum class eMonsterType
{
	None,
	BigWhite,
	Archer,
	Count
};

struct MonsterGroup
{
	eMonsterType type;
	uint32_t count;
	bool isAttackOption;
	Range rangeX;
	Range rangeY;
};

constexpr std::array<MonsterGroup, 7> MONSTER_GROUPS =
{
	// 0
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 2,
		.isAttackOption = false,
		.rangeX = { .min = -100.0f, .max = 100.0f },
		.rangeY = { .min = -100.0f, .max = 100.0f }
	}, 

	// 1
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 3, 
		.isAttackOption = false,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }

	}, 

	// 2
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.count = 4,
		.isAttackOption = false,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 3
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 1,
		.isAttackOption = true,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 4
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 2, 
		.isAttackOption = true,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 5
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 3, 
		.isAttackOption = true,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 6
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.count = 4,
		.isAttackOption = true,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 
};