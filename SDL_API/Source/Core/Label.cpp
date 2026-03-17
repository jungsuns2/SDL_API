#include "pch.h"
#include "Label.h"
#include "Font.h"

void Label::Finalize()
{
	SDL_DestroyTexture(mTexture);
	SDL_FreeSurface(mSurface);
}

SDL_FPoint Label::GetPosition() const
{
	return mPosition;
}

void Label::SetPosition(const SDL_FPoint position)
{
	mPosition = position;
}

uint32_t Label::GetSize() const
{
	return mSize;
}

void Label::SetSize(const uint32_t size)
{
	mFont->SetSize(size);
	mSize = size;
}

void Label::SetFont(Font* font)
{
	assert(font != nullptr);

	mFont = font;
}

void Label::SetText(SDL_Renderer* renderer, const std::string& text)
{
	assert(renderer != nullptr);

	mSurface = TTF_RenderText_Blended(mFont->GetFont(), text.c_str(), mColor);
	assert(mSurface != nullptr);

	mTexture = SDL_CreateTextureFromSurface(renderer, mSurface);
	assert(mTexture != nullptr);
}

SDL_Color Label::GetColor() const
{
	return mColor;
}

void Label::SetColor(const SDL_Color fg)
{
	mColor = fg;
}

Uint8 Label::GetOpacity() const
{
	return mColor.a;
}

void Label::SetOpacity(const Uint8 opacity)
{
	mColor.a = opacity;
}

SDL_Texture* Label::GetTexture() const
{
	return mTexture;
}

uint32_t Label::GetWidth() const
{
	return mSurface->w;
}

uint32_t Label::GetHeight() const
{
	return mSurface->h;
}