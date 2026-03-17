#pragma once
class Constant
{
public:
	Constant() = default;
	Constant(const Constant&) = delete;
	const Constant operator=(Constant&) = delete;
	~Constant() = default;

public: 
	[[nodiscard]] static Constant& Get();

	[[nodiscard]] uint32_t GetWidth() const;
	[[nodiscard]] uint32_t GetHeight() const;
};

