#include <iostream>
#include <SDL.h>
#include <array>

struct Point_2S
{
	int32_t x;
	int32_t y;
};

bool GetKey(const SDL_Scancode virtualCode);
bool GetKeyDown(const SDL_Scancode virtualCode);
bool GetKeyUp(const SDL_Scancode virtualCode);
void SetKeyState(const SDL_Scancode virtualCode, const bool bPressed);

bool GetMouseButton(const Uint8 virtualMouse);
bool GetMouseButtonDown(const Uint8 virtualMouse);
bool GetMouseButtonUp(const Uint8 virtualMouse);
void SetMouseButtonState(const Uint8 virtualMouse, const bool bPressed);

Point_2S GetMousePosition();
void SetMousePosition(Point_2S mousePosition);

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

static std::array<bool, SDL_NUM_SCANCODES> KeysPressed;
static std::array<bool, SDL_NUM_SCANCODES> PrevKeysPressed;

constexpr size_t SDL_MOUSE_COUNT = 6;
static std::array<bool, SDL_MOUSE_COUNT> MousePressed;
static std::array<bool, SDL_MOUSE_COUNT> PreMousePressed;

Point_2S MousePosition;

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
					SetKeyState(event.key.keysym.scancode, bool(event.type == SDL_KEYDOWN));
					break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					SetMouseButtonState(event.button.button, bool(event.type == SDL_MOUSEBUTTONDOWN));
					break;

				case SDL_MOUSEMOTION:
					SetMousePosition({ event.motion.x, event.motion.y });
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

		if (GetMouseButtonDown(SDL_BUTTON_LEFT))
		{
			printf("Left 클릭\n");
		}
		if (GetMouseButtonDown(SDL_BUTTON_RIGHT))
		{
			printf("Right 클릭\n");
		}

		printf("%d, %d\n", GetMousePosition().x, GetMousePosition().y);


		// 게임 업데이트

		// 렌더링
		SDL_SetRenderDrawColor(renderer, 255, 174, 201, 255);
		SDL_RenderClear(renderer);		// 화면을 지정색으로 채운다.

		SDL_RenderPresent(renderer);	// 화면에 출력한다.

		// 이전 키를 모두 초기화한다.
		PrevKeysPressed = {};
		PreMousePressed = {};
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
	return KeysPressed[virtualCode] and PrevKeysPressed[virtualCode];
}

bool GetKeyUp(const SDL_Scancode virtualCode)
{
	return not KeysPressed[virtualCode] and PrevKeysPressed[virtualCode];
}

void SetKeyState(const SDL_Scancode virtualCode, const bool bPressed)
{
	PrevKeysPressed[virtualCode] = (KeysPressed[virtualCode] != bPressed);
	KeysPressed[virtualCode] = bPressed;
}

bool GetMouseButton(const Uint8 virtualMouse)
{
	return MousePressed[Uint8(virtualMouse)];
}

bool GetMouseButtonDown(const Uint8 virtualMouse)
{
	Uint8 index = Uint8(virtualMouse);
	return MousePressed[index] and PrevKeysPressed[index];
}

bool GetMouseButtonUp(const Uint8 virtualMouse)
{
	Uint8 index = Uint8(virtualMouse);
	return not MousePressed[index] and PrevKeysPressed[index];
}

void SetMouseButtonState(const Uint8 virtualMouse, const bool bPressed)
{
	Uint8 index = Uint8(virtualMouse);
	PrevKeysPressed[index] = (MousePressed[index] != bPressed);
	MousePressed[index] = bPressed;
}

Point_2S GetMousePosition()
{
	return MousePosition;
}

void SetMousePosition(Point_2S mousePosition)
{
	MousePosition = mousePosition;
}