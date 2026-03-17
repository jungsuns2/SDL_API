#include "pch.h"
#include "Core/Core.h"
#include "Core/Input.h"
#include "Game/MainScene.h"

Core gCore;

int main(int argc, char* argv[])
{

	// 초기화를 한다.
	gCore.Initialize(new MainScene);

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
				Input::Get().SetKeyState(event.key.keysym.scancode, bool(event.type == SDL_KEYDOWN));
				break;

			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				Input::Get().SetMouseButtonState(event.button.button, bool(event.type == SDL_MOUSEBUTTONDOWN));
				break;

			case SDL_MOUSEMOTION:
				Input::Get().SetMousePosition({ float(event.motion.x), float(event.motion.y) });
				break;

			case SDL_QUIT:
				quit = true;
				goto EXIT_WINDOW;
				break;

			default:
				break;
			}
		}

		// 게임 업데이트
		if (not gCore.Update(deltaTime))
		{
			break;
		}

		// 이전 키를 모두 초기화한다.
		Input::Get().Clear();

		deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() * 0.001f;
	}

EXIT_WINDOW:

	// 헤제를 한다.
	gCore.Finalize();

	return 0;
}