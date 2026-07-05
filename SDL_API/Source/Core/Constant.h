#pragma once

class Constant final
{
public:
	Constant() = default;
	Constant(const Constant&) = delete;
	const Constant operator=(Constant&) = delete;
	~Constant() = default;

public: 
	[[nodiscard]] static const Constant& Get();

	[[nodiscard]] uint32_t GetWidth() const;
	[[nodiscard]] uint32_t GetHeight() const;

	[[nodiscard]] float GetHalfWidth() const;
	[[nodiscard]] float GetHalfHeight() const;

public:
	static constexpr uint32_t MAX_LAYER = 6;
};