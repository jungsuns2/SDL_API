#include "pch.h"
#include "Input.h"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

static Input input;
bool isMousePositionPrint;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("Keyboard", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	bool quit = false;
	SDL_Event event{};

	while (not quit)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					input.SetKeyState(event.key.keysym.scancode, bool(event.type == SDL_KEYDOWN));
					break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					input.SetMouseButtonState(event.button.button, bool(event.type == SDL_MOUSEBUTTONDOWN));
					break;

				case SDL_MOUSEMOTION:
					input.SetMousePosition({ event.motion.x, event.motion.y });
					break;

				case SDL_QUIT:
					quit = true;
					break;

				default:
					break;
			}
		}

		// 키 입력 업데이트
		if (input.GetKey(SDL_SCANCODE_W))
		{
			printf("W 이동\n");
		}
		if (input.GetKey(SDL_SCANCODE_S))
		{
			printf("S 이동\n");
		}
		if (input.GetKey(SDL_SCANCODE_A))
		{
			printf("A 이동\n");
		}
		if (input.GetKey(SDL_SCANCODE_D))
		{
			printf("D 이동\n");
		}
		if (input.GetKeyDown(SDL_SCANCODE_SPACE))
		{
			printf("Space 점프\n");
		}

		if (input.GetKeyDown(SDL_SCANCODE_ESCAPE))
		{
			printf("ESC 종료\n");
			quit = true;
		}

		if (input.GetMouseButtonDown(SDL_BUTTON_LEFT))
		{
			printf("Left 클릭\n");
		}
		if (input.GetMouseButtonDown(SDL_BUTTON_RIGHT))
		{
			printf("Right 클릭\n");
		}

		if (input.GetKey(SDL_SCANCODE_T))
		{
			isMousePositionPrint = !isMousePositionPrint;
		}

		if (isMousePositionPrint)
		{
			printf("%d, %d\n", input.GetMousePosition().x, input.GetMousePosition().y);
		}


		// 게임 업데이트

		// 렌더링
		SDL_SetRenderDrawColor(renderer, 255, 174, 201, 255);
		SDL_RenderClear(renderer);		// 화면을 지정색으로 채운다.

		SDL_RenderPresent(renderer);	// 화면에 출력한다.

		// 이전 키를 모두 초기화한다.
		input.Clear();
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}