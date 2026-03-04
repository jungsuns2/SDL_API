#include <iostream>
#include <SDL.h>
#include <assert.h>

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

int main(int argc, char* argv[])
{
	// SDL 초기화
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cout << SDL_GetError() << std::endl;
		return 0;
	}

	// 윈도우 생성
	SDL_Window* pWindow = SDL_CreateWindow("bmp", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	// 렌더러 생성
	SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, 0);

	// bmp 이미지 생성
	SDL_Surface* pHellowBMP = SDL_LoadBMP("hello.bmp");
	assert(pHellowBMP != nullptr);

	// 텍스처 객체로 변환
	SDL_Texture* pTexture = SDL_CreateTextureFromSurface(pRenderer, pHellowBMP);
	assert(pTexture != nullptr);
	SDL_FreeSurface(pHellowBMP);

	bool running = true;
	SDL_Event event;

	// 이벤트 처리
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
			}
			else if (event.type == SDL_QUIT)
			{
				running = false;
			}
		}

		SDL_RenderClear(pRenderer);
		SDL_RenderCopy(pRenderer, pTexture, nullptr, nullptr);
		SDL_RenderPresent(pRenderer);
	}

	SDL_DestroyTexture(pTexture);
	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}