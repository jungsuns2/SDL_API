#include "pch.h"
#include "Helper.h"

void Helper::Initialize(SDL_Renderer* renderer)
{
	assert(renderer != nullptr);

	mRenderer = renderer;
}

SDL_Renderer* Helper::GetRenderer() const
{
	return mRenderer;
}