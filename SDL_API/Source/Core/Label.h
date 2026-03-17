#pragma once

class Font;

class Label final
{
public:
	Label() = default;
	Label(const Label&) = delete;
	const Label& operator=(Label&) = delete;
	~Label() = default;

public:
	void Finalize();

public:
	[[nodiscard]] SDL_FPoint GetPosition() const;
	void SetPosition(const SDL_FPoint position);

	[[nodiscard]] uint32_t GetSize() const;
	void SetSize(const uint32_t scale);

	void SetFont(Font* font);
	void SetText(SDL_Renderer* renderer, const std::string& text);

	[[nodiscard]] SDL_Color GetColor() const;
	void SetColor(const SDL_Color fg);

	[[nodiscard]] Uint8 GetOpacity() const;
	void SetOpacity(const Uint8 opacity);

	[[nodiscard]] uint32_t GetWidth() const;
	[[nodiscard]] uint32_t GetHeight() const;

	SDL_Texture* GetTexture() const;

private:
	SDL_Surface* mSurface = nullptr;
	SDL_Texture* mTexture = nullptr;

	Font* mFont = nullptr;

	SDL_FPoint mPosition{};
	uint32_t mSize = 30;
	SDL_Color mColor{ .a = 255 };
};

