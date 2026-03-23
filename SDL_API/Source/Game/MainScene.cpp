#include "pch.h"
#include "Core/Entity/EntityWorld.h"
#include "Core/Helper.h"
#include "Core/Input.h"
#include "MainScene.h"

void MainScene::Initialize()
{
	{
		mFont.Initilize("Resource/DroidSans.TTF", 50);
		mPlayerTexture.Initialize(GetHelper(), "Resource/Player.png");
	}

	{
		Transform transform;
		transform.position = { .x = 0.0f, .y = 0.0f };
		mLabel.AddComponent(transform);

		Label label;
		label.font = &mFont;
		label.SetText(GetHelper(), "UI Label");
		mLabel.AddComponent(label);

		GetEntityWorld()->AddEntity(&mLabel);
	}

	{
		Transform transform;
		transform.position = { 100.0, 100.0f};
		mPlayer.AddComponent(transform);

		Material material;
		material.texture = &mPlayerTexture;
		mPlayer.AddComponent(material);

		mMainCamera.position = {};
		mPlayer.AddComponent(mMainCamera);
		SetCamera(&mMainCamera);

		GetEntityWorld()->AddEntity(&mPlayer);
	}

	{
		Transform transform;
		transform.position = { .x = 100.0f, .y = 0.0f };
		transform.scale = { 0.5f, 0.5f };
		mMonster.AddComponent(transform);

		Material material;
		material.texture = &mPlayerTexture;
		mMonster.AddComponent(material);

		GetEntityWorld()->AddEntity(&mMonster);
	}
}

bool MainScene::Update(const float deltaTime)
{
	// 키 입력을 처리한다.
	{
		if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
		{
			printf("ESC 종료\n");
			return false;
		}

		if (Input::Get().GetMouseButtonDown(SDL_BUTTON_LEFT))
		{
			printf("Left 클릭\n");
		}
		if (Input::Get().GetMouseButtonDown(SDL_BUTTON_RIGHT))
		{
			printf("Right 클릭\n");
		}

		static bool mousePositionPrint;

		if (Input::Get().GetKeyDown(SDL_SCANCODE_T))
		{
			mousePositionPrint = !mousePositionPrint;
		}

		if (mousePositionPrint)
		{
			printf("%f, %f\n", Input::Get().GetMousePosition().x, Input::Get().GetMousePosition().y);
		}

		// 플레이어 좌표를 업데이트한다.
		{
			const int32_t moveX = Input::Get().GetKey(SDL_SCANCODE_D) - Input::Get().GetKey(SDL_SCANCODE_A);
			const int32_t moveY = Input::Get().GetKey(SDL_SCANCODE_S) - Input::Get().GetKey(SDL_SCANCODE_W);

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

			Transform* transform = mPlayer.GetComponent<Transform>();
			transform->position.x += velocity.x * deltaTime;
			transform->position.y += velocity.y * deltaTime;
		}

		// 카메라를 업데이트한다.
		Transform* transform = mPlayer.GetComponent<Transform>();
		mMainCamera.position = transform->position;
	}

	return true;
}

void MainScene::Finalize()
{
	mFont.Finalize();
	mPlayerTexture.Finalize();
}
