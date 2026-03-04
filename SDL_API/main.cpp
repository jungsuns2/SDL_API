#include <iostream>
#include <SDL.h>

SDL_Color green{ 34, 177, 76, 0 };
SDL_Color purple{ 185, 151, 230, 0 };

int main(int argc, char* argv[])
{
	// SDL 초기화
	SDL_Init(SDL_INIT_EVERYTHING);

	int width = 640;
	int height = 480;

	// 윈도우 생성
	SDL_Window* pWindow = SDL_CreateWindow("Keyboard Input", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_SHOWN);

	// 렌더러 생성
	SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, -1, 0);

	bool quit = false;
	SDL_Event event;

	// 이벤트 처리
	while (not quit)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					quit = true;
					break;

				case SDL_KEYDOWN:
					std::cout << "Key pressed: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
					break;

				case SDL_KEYUP:
					std::cout << "Key released: " << SDL_GetKeyName(event.key.keysym.sym) << std::endl;
					break;

				default:
					break;
			}
		}

		SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
		
		// 텍스처 초기화
		SDL_RenderClear(pRenderer);

		// 화면에 출력
		SDL_RenderPresent(pRenderer);
	}

	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}