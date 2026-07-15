#pragma once

constexpr size_t WAVE_COUNT = 20;
constexpr size_t  MONSTER_GROUP_COUNT = 25;

struct WaveDesc
{
	float elapsedTime; // const
	std::array<uint32_t, MONSTER_GROUP_COUNT> monsterGroupIndicies;// const
	uint32_t monsterGroupCount;
	float monsterGroupSpawnIntervalTime; // const


};

constexpr std::array<WaveDesc, WAVE_COUNT> WAVES =
{
	// 0
	WaveDesc
	{
		.elapsedTime = 1.0f,
		.monsterGroupIndicies = { 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		.monsterGroupCount = 14,
		.monsterGroupSpawnIntervalTime = 3.0f
	},

	// 1
	WaveDesc
	{
		.elapsedTime = 1.0f,
		.monsterGroupIndicies = { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0 },
		.monsterGroupCount = 21,
		.monsterGroupSpawnIntervalTime = 3.0f
	},
	
	// 2
	WaveDesc
	{
		.elapsedTime = 60.0f,
		.monsterGroupIndicies = { 4, 2, 1,  4, 2, 1,  4, 2, 1,  4, 2, 1,  4, 2, 1 },
		.monsterGroupCount = 15,
		.monsterGroupSpawnIntervalTime = 4.0f
	},

	// 3
	WaveDesc
	{
		.elapsedTime = 70.0f,
		.monsterGroupIndicies = { 5, 2, 4,  5, 2, 4,  5, 2, 4,  5, 2, 4,  5, 2, 4,  5, 2, 4 },
		.monsterGroupCount = 18,
		.monsterGroupSpawnIntervalTime = 4.0f
	},

	// 4
	WaveDesc
	{
		.elapsedTime = 80.0f,
		.monsterGroupIndicies = { 6, 2, 5,  6, 2, 5,  6, 2, 5,  6, 2, 5,  6, 2, 5,  6 },
		.monsterGroupCount = 16,
		.monsterGroupSpawnIntervalTime = 5.0f
	},

	// 5
	WaveDesc
	{
		.elapsedTime = 90.0f,
		.monsterGroupIndicies = { 7, 3, 6,  7, 3, 6,  7, 3, 6,  7, 3, 6,  7, 3, 6,  7, 3, 6 },
		.monsterGroupCount = 18,
		.monsterGroupSpawnIntervalTime = 5.0f
	},

	// 6
	WaveDesc
	{
		.elapsedTime = 100.0f,
		.monsterGroupIndicies = { 8, 7, 9, 5,  8, 7, 9, 5,  8, 7, 9, 5,  8, 7, 9, 5,  8 },
		.monsterGroupCount = 17,
		.monsterGroupSpawnIntervalTime = 6.0f
	},

	// 7
	WaveDesc
	{
		.elapsedTime = 110.0f,
		.monsterGroupIndicies = { 9, 10, 6, 7,  9, 10, 6, 7,  9, 10, 6, 7,  9, 10, 6, 7,  9, 10, 6, 7 },
		.monsterGroupCount = 19,
		.monsterGroupSpawnIntervalTime = 6.0f
	},

	// 8
	WaveDesc
	{
		.elapsedTime = 120.0f,
		.monsterGroupIndicies = { 11, 7, 10, 6,  11, 7, 10, 6,  11, 7, 10, 6,  11, 7, 10, 6,  11, 7 },
		.monsterGroupCount = 18,
		.monsterGroupSpawnIntervalTime = 7.0f
	},

	// 9
	WaveDesc
	{
		.elapsedTime = 130.0f,
		.monsterGroupIndicies = { 11,  12,  1, 5, 9,  1, 5, 9,  1, 5, 9,  1, 5, 9,  1, 5, 9,  1, 5, 9 },
		.monsterGroupCount = 19,
		.monsterGroupSpawnIntervalTime = 7.0f
	},
};