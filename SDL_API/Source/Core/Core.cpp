#include "pch.h"
#include "Core.h"
#include "Constant.h"

void Core::Initialize()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	mWindow = SDL_CreateWindow("Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		Constant::Get().GetWidth(), Constant::Get().GetHeight(), SDL_WINDOW_SHOWN);

	// SDL_RENDERER_PRESENTVSYNC: 모니터 주사율에 맞춘다.
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	TTF_Init();
}

bool Core::Update()
{
	return true;
}

void Core::Finalize()
{
	IMG_Quit();

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);


	SDL_Quit();
}

SDL_Window* Core::GetWindow() const
{
	return mWindow;
}

SDL_Renderer* Core::GetRender() const
{
	return mRenderer;
}
