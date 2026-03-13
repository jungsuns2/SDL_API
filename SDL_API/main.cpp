#include "pch.h"
#include "Input.h"

struct Player
{
	SDL_Surface* surface;
	SDL_Texture* texture;
	SDL_FPoint position;
};

struct Font
{
	TTF_Font* font;
	SDL_Surface* surface;
	SDL_Texture* texture;
	SDL_FPoint position;
};

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

static Input gInput;
static bool gIsMousePositionPrint;

static Player gPlayer;
static SDL_FRect gPlayerDstFRect;

static Font gFont;
static SDL_FRect gTextDstFRect;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

	// SDL_RENDERER_PRESENTVSYNC: 모니터 주사율에 맞춘다.
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	
	if (TTF_Init() == -1)
	{
		return 0;
	}

	// 초기화를 한다.
	{
		// 폰트
		{
			TTF_Font* font = TTF_OpenFont("Resource/DroidSans.TTF", 30);
			assert(font != nullptr);

			SDL_Surface* surface = TTF_RenderText_Blended(font, "TTF Test", { .r = 0, .g = 0, .b = 0 });
			assert(surface != nullptr);

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			assert(texture != nullptr);

			gFont.font = font;
			gFont.surface = surface;
			gFont.texture = texture;
			gFont.position = { .x = 10.0f, .y = 10.0f };
		}

		// 플레이어
		{
			SDL_Surface* surface = IMG_Load("Resource/player.png");
			assert(surface != nullptr);

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			assert(texture != nullptr);

			gPlayer.surface = surface;
			gPlayer.texture = texture;
			gPlayer.position = {};
		}
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
					gInput.SetKeyState(event.key.keysym.scancode, bool(event.type == SDL_KEYDOWN));
					break;

				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					gInput.SetMouseButtonState(event.button.button, bool(event.type == SDL_MOUSEBUTTONDOWN));
					break;

				case SDL_MOUSEMOTION:
					gInput.SetMousePosition({ float(event.motion.x), float(event.motion.y) });
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
			// 키 입력을 처리한다.
			{
				if (gInput.GetKeyDown(SDL_SCANCODE_ESCAPE))
				{
					printf("ESC 종료\n");
					quit = true;
				}

				if (gInput.GetMouseButtonDown(SDL_BUTTON_LEFT))
				{
					printf("Left 클릭\n");
				}
				if (gInput.GetMouseButtonDown(SDL_BUTTON_RIGHT))
				{
					printf("Right 클릭\n");
				}

				if (gInput.GetKey(SDL_SCANCODE_T))
				{
					gIsMousePositionPrint = !gIsMousePositionPrint;
				}

				if (gIsMousePositionPrint)
				{
					printf("%f, %f\n", gInput.GetMousePosition().x, gInput.GetMousePosition().y);
				}
			}

			// 플레이어 좌표를 업데이트한다.
			{
				const int32_t moveX = gInput.GetKey(SDL_SCANCODE_D) - gInput.GetKey(SDL_SCANCODE_A);
				const int32_t moveY = gInput.GetKey(SDL_SCANCODE_S) - gInput.GetKey(SDL_SCANCODE_W);

				static int32_t prevMoveX;
				static int32_t prevMoveY;

				SDL_FPoint velocity = {};
				constexpr float MAX_SPEED = 500.0f;
				constexpr float ACC = 40.0f;

				if (moveX != 0)
				{
					velocity.x = std::clamp(velocity.x + ACC * moveX, -MAX_SPEED, MAX_SPEED);
					prevMoveX = moveX;
				}
				else
				{
					if (prevMoveX > 0)
					{
						velocity.x = std::max(velocity.x - ACC, 0.0f);
					}
					else
					{
						velocity.x = std::min(velocity.x + ACC, 0.0f);
					}
				}

				if (moveY != 0)
				{
					velocity.y = std::clamp(velocity.y + ACC * moveY, -MAX_SPEED, MAX_SPEED);
					prevMoveY = moveY;
				}
				else
				{
					if (prevMoveY > 0)
					{
						velocity.y = std::max(velocity.y - ACC, 0.0f);
					}
					else
					{
						velocity.y = std::min(velocity.y + ACC, 0.0f);
					}
				}

				float length = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
				if (length > 0.0f)
				{
					SDL_FPoint direction = { .x = velocity.x / length, .y = velocity.y / length };

					velocity.x = MAX_SPEED * direction.x;
					velocity.y = MAX_SPEED * direction.y;
				}

				gPlayer.position.x += velocity.x * deltaTime;
				gPlayer.position.y += velocity.y * deltaTime;
			}

			// 렌더링할 이미지 크기를 조정한다.
			{
				gTextDstFRect =
				{
					.x = gFont.position.x,
					.y = gFont.position.y,
					.w = float(gFont.surface->w),
					.h = float(gFont.surface->h)
				};

				gPlayerDstFRect =
				{
					.x = gPlayer.position.x,
					.y = gPlayer.position.y,
					.w = float(gPlayer.surface->w),
					.h = float(gPlayer.surface->h)
				};
			}
		}

		// 렌더링을 한다.
		{
			SDL_SetRenderDrawColor(renderer, 255, 174, 201, 255);
			SDL_RenderClear(renderer);		// 화면을 지정색으로 채운다.

			SDL_RenderCopyF(renderer, gFont.texture, nullptr, &gTextDstFRect);
			SDL_RenderCopyF(renderer, gPlayer.texture, nullptr, &gPlayerDstFRect);	// 플레이어를 출력한다.

			SDL_RenderPresent(renderer);	// 화면에 출력한다.
		}

		// 이전 키를 모두 초기화한다.
		gInput.Clear();

		deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() * 0.001f;
	}

	// 헤제를 한다.
	SDL_DestroyTexture(gFont.texture);
	SDL_DestroyTexture(gPlayer.texture);

	SDL_FreeSurface(gPlayer.surface);
	SDL_FreeSurface(gFont.surface);

	IMG_Quit();
	TTF_CloseFont(gFont.font);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}