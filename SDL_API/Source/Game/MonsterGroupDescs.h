#pragma once

enum class eMonsterType
{
	None,
	BigWhite,
	Archer,
	SkelDog,
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
	eAttackType attackType;
	uint32_t count;
	Range rangeX;
	Range rangeY;
};

constexpr std::array<MonsterGroup, 11> MONSTER_GROUPS =
{
	// 0
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.attackType = eAttackType::Melee,
		.count = 1,
		.rangeX = { .min = -100.0f, .max = 100.0f },
		.rangeY = { .min = -100.0f, .max = 100.0f }
	}, 

	// 1
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.attackType = eAttackType::Melee,
		.count = 2,
		.rangeX = { .min = -100.0f, .max = 100.0f },
		.rangeY = { .min = -100.0f, .max = 100.0f }
	}, 

	// 2
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.attackType = eAttackType::Melee,
		.count = 3, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }

	}, 

	// 3
	MonsterGroup
	{
		.type = eMonsterType::BigWhite,
		.attackType = eAttackType::Melee,
		.count = 4,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 4
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.attackType = eAttackType::Range,
		.count = 1,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 5
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.attackType = eAttackType::Range,
		.count = 2, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 6
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.attackType = eAttackType::Range,
		.count = 3, 
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 7
	MonsterGroup
	{
		.type = eMonsterType::Archer,
		.attackType = eAttackType::Range,
		.count = 4,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 8
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.attackType = eAttackType::Melee,
		.count = 1,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 9
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.attackType = eAttackType::Melee,
		.count = 2,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 

	// 10
	MonsterGroup
	{
		.type = eMonsterType::SkelDog,
		.attackType = eAttackType::Melee,
		.count = 3,
		.rangeX = {.min = -100.0f, .max = 100.0f },
		.rangeY = {.min = -100.0f, .max = 100.0f }
	}, 
};