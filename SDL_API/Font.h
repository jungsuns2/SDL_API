#pragma once
class Font final
{
public:
	Font() = default;
	Font(const Font& other) = delete;
	Font& operator=(const Font&) = delete;
	~Font() = default;

public:
	void Initilize(SDL_Renderer* renderer, const std::string fileName);
	void Finalize();

public:
	TTF_Font* GetFont() const;
	void SetSize(const uint32_t size);

private:
	TTF_Font* mFont = nullptr;
	uint32_t mSize = 30;
};

