#include <iostream>
#include <SDL.h>

int main(int argc, char* argv[]) 
{
	SDL_Window* window = nullptr;

	// SDL 초기화
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL Initialization Fail: %s\n", SDL_GetError());
		return 1;
	}

	// 윈도우 창 생성
	window = SDL_CreateWindow("title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
		640, 480, SDL_WINDOW_SHOWN);

	if (!window) 
	{
		printf("SDL Initialization Fail: %s\n", SDL_GetError());
		SDL_Quit();

		return 1;
	}


	// 메시지 루프
	SDL_Event event;
	int quit = 0;

	while (not quit) 
	{
		while (SDL_PollEvent(&event)) 
		{
			if (event.type == SDL_QUIT) 
			{
				quit = 1;
			}
		}
	}

	// 종료
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}