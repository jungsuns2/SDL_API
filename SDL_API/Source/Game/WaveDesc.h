#pragma once

struct WaveDesc
{
	float elapsedTime; // const
	std::array<uint32_t, 20> monsterGroupIndicies;// const
	uint32_t monsterGroupCount;
	float monsterGroupSpawnIntervalTime; // const


};

constexpr std::array<WaveDesc, 20> WAVES =
{
	// 0
	WaveDesc
	{
		.elapsedTime = 20.0f,
		.monsterGroupIndicies = { 11, 5, 6, 5 },
		.monsterGroupCount = 4,
		.monsterGroupSpawnIntervalTime = 3.0f
	},

	// 1
	WaveDesc
	{
		.elapsedTime = 25.0f,
		.monsterGroupIndicies = { 0, 3, 0, 3, 0, 3, 0, 3, 0, 3 },
		.monsterGroupCount = 10,
		.monsterGroupSpawnIntervalTime = 3.0f
	},
	
	// 2
	WaveDesc
	{
		.elapsedTime = 30.0f,
		.monsterGroupIndicies = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
		.monsterGroupCount = 10,
		.monsterGroupSpawnIntervalTime = 3.0f
	},

	// 3
	WaveDesc
	{
		.elapsedTime = 35.0f,
		.monsterGroupIndicies = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
		.monsterGroupCount = 10,
		.monsterGroupSpawnIntervalTime = 3.0f
	},
};

//constexpr std::array<WaveDesc, 20> WAVES =
//{
//	// 0
//	WaveDesc
//	{
//		.elapsedTime = 20.0f, 
//		.monsterGroupIndicies = { 0, 3, 0, 3, 0, 3, 0, 3, 0, 3 },
//		.monsterGroupCount = 10, 
//		.monsterGroupSpawnIntervalTime = 3.0f
//	},
//
//	// 1
//	WaveDesc
//	{
//		.elapsedTime = 25.0f, 
//		.monsterGroupIndicies = { 0, 3, 0, 0, 3 },
//		.monsterGroupCount = 5, 
//		.monsterGroupSpawnIntervalTime = 3.0f
//	},
//
//	// 2
//	WaveDesc
//	{
//		.elapsedTime = 30.0f, 
//		.monsterGroupIndicies = { 1, 3, 1, 3, 1, 3 },
//		.monsterGroupCount = 6, 
//		.monsterGroupSpawnIntervalTime = 3.5f
//	},
//
//	// 3
//	WaveDesc
//	{
//		.elapsedTime = 35.0f, 
//		.monsterGroupIndicies = { 1, 3, 1, 3, 1, 3 },
//		.monsterGroupCount = 6, 
//		.monsterGroupSpawnIntervalTime = 3.5f
//	},
//
//
//};