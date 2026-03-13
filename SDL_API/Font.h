#pragma once
class Font final
{
public:
	Font() = default;
	Font(const Font& other) = delete;
	Font& operator=(const Font&) = delete;
	~Font() = default;

public:
	void Initilize(SDL_Renderer* renderer, const std::string fileName, const uint32_t size);
	void Finalize();

public:
	TTF_Font* GetFont() const;

private:
	TTF_Font* mFont = nullptr;
	SDL_Surface* surface;
	SDL_Texture* texture;

};

