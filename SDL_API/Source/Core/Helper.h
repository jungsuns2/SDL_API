#pragma once

class Helper final
{
public:
	Helper() = default;
	Helper(const Helper&) = delete;
	const Helper& operator=(Helper&) = delete;
	~Helper() = default;

public:
	void Initialize(SDL_Renderer* renderer);

	[[nodiscard]] SDL_Renderer* GetRenderer() const;

private:
	SDL_Renderer* mRenderer = nullptr;
};