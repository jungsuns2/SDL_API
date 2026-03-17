#include "pch.h"
#include "Font.h"

void Font::Initilize(const std::string& filename)
{
	mFont = TTF_OpenFont(filename.c_str(), mSize);
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

void Font::_SetSize(const uint32_t size)
{
	mSize = size;
}