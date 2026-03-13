#include "pch.h"
#include "Font.h"

void Font::Initilize(SDL_Renderer* renderer, const std::string fileName)
{
	assert(renderer != nullptr);

	mFont = TTF_OpenFont(fileName.c_str(), mSize);
	assert(mFont != nullptr);
}

void Font::Finalize()
{
	TTF_CloseFont(mFont);
}

TTF_Font* Font::GetFont() const
{
	return mFont;
}

void Font::SetSize(const uint32_t size)
{
	mSize = size;
}