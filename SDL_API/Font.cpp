#include "pch.h"
#include "Font.h"

void Font::Initilize(SDL_Renderer* renderer, const std::string fileName, const uint32_t size)
{
	assert(renderer != nullptr);

	mFont = TTF_OpenFont(fileName.c_str(), size);
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
