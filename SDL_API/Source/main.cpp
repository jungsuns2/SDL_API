#include "pch.h"
#include "Core/Core.h"
#include "Core/Input.h"
#include "Game/MainScene.h"
#include "Core/Texture.h"

struct Player
{
	SDL_FPoint position;
};

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

Core gCore;

static Input gInput;
static bool gIsMousePositionPrint;

static Texture gPlayerTexture;
static Player gPlayer;
static SDL_FRect gPlayerDstFRect;

int main(int argc, char* argv[])
{

	// 초기화를 한다.
	{
		gCore.Initialize(new MainScene);

		// 폰트
		{
		}

		// 플레이어
		{
			//gPlayerTexture.Initialize(gCore.GetRender(), "Resource/Player.png");
			//gPlayer.position = {};
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
			if (gCore.Update(deltaTime))
			{
				// 씬 전환 X
			}

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

				//gPlayer.position.x += velocity.x * deltaTime;
				//gPlayer.position.y += velocity.y * deltaTime;
			}

			// 렌더링할 이미지 크기를 조정한다.
			{
				//gPlayerDstFRect =
				//{
				//	.x = gPlayer.position.x,
				//	.y = gPlayer.position.y,
				//	.w = float(gPlayerTexture.GetWidth()),
				//	.h = float(gPlayerTexture.GetHeight())
				//};
			}
		}

		// 렌더링을 한다.
		{
			//SDL_SetRenderDrawColor(gCore.GetRender(), 255, 174, 201, 255);
			//SDL_RenderClear(gCore.GetRender());		// 화면을 지정색으로 채운다.

			//SDL_RenderCopyF(gCore.GetRender(), gLabel.GetTexture(), nullptr, &gTextDstFRect);			// 텍스트를 출력한다.
			//SDL_RenderCopyF(gCore.GetRender(), gPlayerTexture.GetTexture(), nullptr, &gPlayerDstFRect);	// 플레이어를 출력한다.

			//SDL_RenderPresent(gCore.GetRender());	// 화면에 출력한다.
		}

		// 이전 키를 모두 초기화한다.
		gInput.Clear();

		deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() * 0.001f;
	}

	// 헤제를 한다.
	//gPlayerTexture.Finalize();

	gCore.Finalize();

	return 0;
}