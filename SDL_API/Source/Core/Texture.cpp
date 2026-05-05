#include "pch.h"
#include "Font.h"
#include "Helper.h"
#include "Texture.h"

void Texture::Initialize(Helper* helper, const std::string& filename)
{
	assert(helper != nullptr);

	mSurface = IMG_Load(filename.c_str());
	assert(mSurface != nullptr and "이미지를 찾을 수 없습니다.");

	mTexture = SDL_CreateTextureFromSurface(helper->GetRenderer(), mSurface);
}

void Texture::Finalize()
{
	SDL_DestroyTexture(mTexture);
	SDL_FreeSurface(mSurface);
}

uint32_t Texture::GetWidth() const
{
	return mSurface->w;
}

uint32_t Texture::GetHeight() const
{
	return mSurface->h;
}

SDL_Texture* Texture::GetTexture() const
{
	return mTexture;
}