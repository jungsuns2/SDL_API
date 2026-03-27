#pragma once

class GameComponent
{
public:
	GameComponent() = default;
	GameComponent(const uint32_t* id);
	~GameComponent() = default;

public:
	const uint32_t* ID = 0;
};

