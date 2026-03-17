#pragma once

class Core final
{
public:
	Core() = default;
	Core(const Core&) = delete;
	const Core& operator=(Core&) = delete;
	~Core() = default;

public:
	void Initialize();
	bool Update();
	void Finalize();

public:
	[[nodiscard]]  SDL_Window* GetWindow() const;
	[[nodiscard]]  SDL_Renderer* GetRender() const;

private:
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;
};