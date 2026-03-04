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
	SDL_Window* pWindow = SDL_CreateWindow("Mouse Input", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						std::cout << "Mouse pressed: " << static_cast<int>(event.button.button) << std::endl;
						std::cout << "Mouse position: (" << event.button.x << ", " << event.button.y << ")" << std::endl;
					}
					break;

				case SDL_MOUSEBUTTONUP:
					if (event.button.button == SDL_BUTTON_LEFT)
					{
						std::cout << "Mouse released: " << static_cast<int>(event.button.button) << std::endl;
						std::cout << "Mouse position: (" << event.button.x << ", " << event.button.y << ")" << std::endl;
					}
					break;

				case SDL_MOUSEMOTION:
					std::cout << "Mouse moved: (" << event.motion.x << ", " << event.motion.y << ")" << std::endl;
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