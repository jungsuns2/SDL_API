#include "pch.h"
#include "Font.h"

void Font::Initilize(const std::string& filename, const uint32_t size)
{
	mFont = TTF_OpenFont(filename.c_str(), size);
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