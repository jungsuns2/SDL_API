#include <iostream>
#include <SDL.h>
#include <vector>
#include <array>

bool GetKey(const SDL_Scancode virtualCode);
bool GetKeyDown(const SDL_Scancode virtualCode);
bool GetKeyUp(const SDL_Scancode virtualCode);
void SetKeyState(const SDL_Scancode virtualCode, const bool bPressed);

static std::array<size_t, SDL_NUM_SCANCODES> KeysPressed;
static std::array<size_t, SDL_NUM_SCANCODES> KeysStateChanged;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	constexpr int WIDTH = 640;
	constexpr int HEIGHT = 480;

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
					SetKeyState(event.key.keysym.scancode, bool(event.type == SDL_KEYDOWN));
					break;

				case SDL_QUIT:
					quit = true;
					break;

				default:
					break;
			}
		}

		// 키 입력 업데이트
		if (GetKey(SDL_SCANCODE_W))
		{
			printf("W 이동\n");
		}
		if (GetKey(SDL_SCANCODE_S))
		{
			printf("S 이동\n");
		}
		if (GetKey(SDL_SCANCODE_A))
		{
			printf("A 이동\n");
		}
		if (GetKey(SDL_SCANCODE_D))
		{
			printf("D 이동\n");
		}
		if (GetKeyDown(SDL_SCANCODE_SPACE))
		{
			printf("Space 점프\n");
		}

		if (GetKeyDown(SDL_SCANCODE_ESCAPE))
		{
			printf("ESC 종료\n");
			quit = true;
		}

		// 게임 업데이트

		// 렌더링
		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderClear(renderer);		// 화면을 지정색으로 채운다.

		SDL_RenderPresent(renderer);	// 화면에 출력한다.

		// 모든 키를 초기화 한다.
		KeysStateChanged = {};
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

bool GetKey(const SDL_Scancode virtualCode)
{
	return KeysPressed[virtualCode];
}

bool GetKeyDown(const SDL_Scancode virtualCode)
{
	return KeysStateChanged[virtualCode] and KeysPressed[virtualCode];
}

bool GetKeyUp(const SDL_Scancode virtualCode)
{
	return KeysStateChanged[virtualCode] and not KeysPressed[virtualCode];

}

void SetKeyState(const SDL_Scancode virtualCode, const bool bPressed)
{
	KeysStateChanged[virtualCode] = (KeysPressed[virtualCode] != bPressed);
	KeysPressed[virtualCode] = bPressed;
}

