#include <iostream>
#include <SDL.h>

SDL_Color green{ 34, 177, 76, 0 };
SDL_Color purple{ 185, 151, 230, 0 };

int main(int argc, char* argv[])
{
	SDL_Window* pWindow = nullptr;
	SDL_Renderer* pRenderer = nullptr;


	int width = 640;
	int height = 480;

	if (SDL_CreateWindowAndRenderer(width, height, 0, &pWindow, &pRenderer) < 0)
	{
		printf("SDL_CreateWindowAndRenderer Error\n");
		return 0;
	}

	SDL_SetWindowTitle(pWindow, "Screen flip");

	bool running = true;
	int lastTickCount = SDL_GetTicks();
	int curTickCount = lastTickCount;
	int color = 0;

	while (running)
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			// ESC 키를 누르면 종료된다.
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

		curTickCount = SDL_GetTicks();

		if (curTickCount - lastTickCount > 1000)
		{
			color++;
			color = color % 2;
			lastTickCount = curTickCount;
		}

		if (color == 0)
		{
			SDL_SetRenderDrawColor(pRenderer, green.r, green.g, green.b, green.a);
		}
		else
		{
			SDL_SetRenderDrawColor(pRenderer, purple.r, purple.g, purple.b, purple.a);
		}

		SDL_RenderClear(pRenderer);
		SDL_RenderFillRect(pRenderer, nullptr);

		SDL_RenderPresent(pRenderer);
	}

	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	return 0;
}