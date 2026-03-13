#pragma once

class Texture final
{
public:
	Texture() = default;
	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture&) = delete;
	~Texture() = default;

public:
	void Initialize(SDL_Renderer* renderer, const std::string fileName);
	void Finalize();

public:
	[[nodiscard]] uint32_t GetWidth() const;
	[[nodiscard]] uint32_t GetHeight() const;

	SDL_Texture* GetTexture() const;

private:
	SDL_Surface* mSurface = nullptr;
	SDL_Texture* mTexture = nullptr;
};