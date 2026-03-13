#include "pch.h"
#include "Texture.h"

void Texture::Initialize(SDL_Renderer* renderer, const std::string fileName)
{
	assert(renderer != nullptr);

	mSurface = IMG_Load(fileName.c_str());
	mTexture = SDL_CreateTextureFromSurface(renderer, mSurface);
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