#pragma once

class Core final
{
public:
	Core() = default;
	Core(const Core& other) = delete;
	Core operator=(Core& other) = delete;
	~Core() = default;

public:
	void Initialize();
	bool Update();
	void Finalize();
};