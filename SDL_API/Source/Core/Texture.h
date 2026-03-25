#pragma once

class Helper;

class Texture final
{
public:
	Texture() = default;
	Texture(const Texture&) = delete;
	const Texture& operator=(const Texture&) = delete;
	~Texture() = default;

public:
	void Initialize(Helper* helper, const std::string& fileName);
	void Finalize();

public:
	[[nodiscard]] uint32_t GetWidth() const;
	[[nodiscard]] uint32_t GetHeight() const;

	[[nodiscard]] SDL_Texture* GetTexture() const;

private:
	SDL_Surface* mSurface = nullptr;
	SDL_Texture* mTexture = nullptr;
};