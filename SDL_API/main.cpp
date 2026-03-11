#include "pch.h"
#include "Input.h"

struct Player
{
	SDL_Surface* imageSurface;
	SDL_Texture* texture;
	SDL_FPoint position;
	SDL_FPoint velocity;
};

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

static Input input;
static bool isMousePositionPrint;

static Player player;

static SDL_FRect dstFRect;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	// SDL_RENDERER_PRESENTVSYNC: 모니터 주사율에 맞춘다.
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	// 초기화를 한다.
	{
		SDL_Surface * imageSurface = IMG_Load("player.png");
		assert(imageSurface != nullptr);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
		assert(texture != nullptr);

		player.imageSurface = imageSurface;
		player.texture = texture;
		player.position = {};
		player.velocity = {};
	}

	float deltaTime = 0.0f;

	bool quit = false;
	SDL_Event event{};

	while (not quit)
	{
		auto startTime = std::chrono::high_resolution_clock::now();

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
					input.SetMousePosition({ float(event.motion.x), float(event.motion.y) });
					break;

				case SDL_QUIT:
					quit = true;
					break;

				default:
					break;
			}
		}

		// 게임 업데이트
		{
			SDL_FPoint velecity{};

			constexpr uint32_t SPEED = 500;
			if (input.GetKey(SDL_SCANCODE_W))
			{
				player.position.y -= SPEED * deltaTime;
			}
			if (input.GetKey(SDL_SCANCODE_S))
			{
				player.position.y += SPEED * deltaTime;
			}
			if (input.GetKey(SDL_SCANCODE_A))
			{
				player.position.x -= SPEED * deltaTime;
			}
			if (input.GetKey(SDL_SCANCODE_D))
			{
				player.position.x += SPEED * deltaTime;
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
				printf("%f, %f\n", input.GetMousePosition().x, input.GetMousePosition().y);
			}
		}

		// 렌더링을 한다.
		{
			SDL_SetRenderDrawColor(renderer, 255, 174, 201, 255);
			SDL_RenderClear(renderer);		// 화면을 지정색으로 채운다.

			dstFRect =
			{
				.x = player.position.x,
				.y = player.position.y,
				.w = float(player.imageSurface->w),
				.h = float(player.imageSurface->h)
			};
			SDL_RenderCopyF(renderer, player.texture, nullptr, &dstFRect);	// 이미지를 출력한다.

			SDL_RenderPresent(renderer);	// 화면에 출력한다.
		}

		// 이전 키를 모두 초기화한다.
		input.Clear();

		deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() * 0.001f;
	}

	// 헤제를 한다.
	SDL_DestroyTexture(player.texture);
	SDL_FreeSurface(player.imageSurface);

	IMG_Quit();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}