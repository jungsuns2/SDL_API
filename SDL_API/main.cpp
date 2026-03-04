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

	// 렌더러 대신 윈도우 객체의 SDL_Surface 객체를 얻는다.
	SDL_Surface* pScreenSurface = SDL_GetWindowSurface(pWindow);

	// bmp 이미지 생성
	SDL_Surface* pHellowBMP = SDL_LoadBMP("hello.bmp");
	assert(pHellowBMP != nullptr);

	// 이벤트 처리
	bool running = true;
	SDL_Event event;

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

		//  직접 로드한 BMP 이미지를 윈도우 객체로 부터 획득한 SDL_Surface 객체에 복사
		SDL_BlitSurface(pHellowBMP, nullptr, pScreenSurface, nullptr);
		
		// 최종적으로 반영
		SDL_UpdateWindowSurface(pWindow);
	}

	SDL_FreeSurface(pHellowBMP);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	// 결론: GPU 메모리같은 하드웨어 방식을 활용한 방식이 아니라 느릴 수 있다.
	// 하드웨어 처리를 지원하지 않는 임베디드 장치에서는 좋다.
	return 0;
}