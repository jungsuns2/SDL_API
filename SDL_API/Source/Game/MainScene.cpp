#include "pch.h"
#include "Core/Entity/EntityWorld.h"

#include "Core/Helper.h"
#include "Core/Input.h"
#include "MainScene.h"

void MainScene::Initialize()
{
	// Texture
	{
		mFont.Initilize("Resource/DroidSans.TTF", 50);
		mMonsterTexture.Initialize(GetHelper(), "Resource/Player.png");

		for (uint32_t i = 0; i < IDLE_COUNT; ++i)
		{
			mPlayerIdleTextures[i].Initialize(GetHelper(), "Resource/Char/Alice/Idle/" + std::to_string(i) + ".png");

			Clip::Frame frame;
			frame.texture = &mPlayerIdleTextures[i];
			frame.durationTime = 0.18f;

			mPlayerClips[uint32_t(PlayerState::Idle)].AddClip(frame);
		}

		for (uint32_t i = 0; i < RUN_COUNT; ++i)
		{
			mPlayerRunTextures[i].Initialize(GetHelper(), "Resource/Char/Alice/Run/" + std::to_string(i) + ".png");

			Clip::Frame frame;
			frame.texture = &mPlayerRunTextures[i];
			frame.durationTime = 0.18f;

			mPlayerClips[uint32_t(PlayerState::Run)].AddClip(frame);
		}
	}

	// Label
	{
		Transform transform;
		transform.position = {};
		mLabel.AddComponent(transform);

		Label label;
		label.font = &mFont;
		label.SetText(GetHelper(), "UI Label");
		mLabel.AddComponent(label);

		GetEntityWorld()->AddEntity(&mLabel);
	}

	// Player
	{
		Transform transform;
		transform.position = {};
		transform.scale = { .width = 3.0f, .height = 3.0f };
		mPlayer.AddComponent(transform);

		mPlayerClips[uint32_t(PlayerState::Idle)].SetLoop(true);
		mPlayerClips[uint32_t(PlayerState::Run)].SetLoop(true);

		Animator animator;
		animator.clipState = &mPlayerClips[uint32_t(PlayerState::Idle)];
		animator.elapsedTime = 0.0f;
		mPlayer.AddComponent(animator);

		mMainCamera.position = {};
		mPlayer.AddComponent(mMainCamera);
		SetCamera(&mMainCamera);

		GetEntityWorld()->AddEntity(&mPlayer);
	}

	// Monster
	{
		Transform transform;
		transform.position = { .x = -200.0f, .y = 0.0f };
		transform.scale = { 0.5f, 0.5f };
		mMonster.AddComponent(transform);

		Material material;
		material.texture = &mMonsterTexture;
		mMonster.AddComponent(material);

		GetEntityWorld()->AddEntity(&mMonster);
	}
}

bool MainScene::Update(const float deltaTime)
{		
	// 카메라를 업데이트한다.
	{
		Transform* target = mPlayer.GetComponent<Transform>();
		Point offset = { .x = 30.0f, .y = 10.0f };
		mMainCamera.position = Math::AddVector(target->position, offset);
	}

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

			Point velocity = {};
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
				Point direction = { .x = velocity.x / length, .y = velocity.y / length };

				velocity = Math::ScaleVector(direction, MAX_SPEED);
			}
			
			mPlayerState = (length != 0.0f) ? PlayerState::Run : PlayerState::Idle;

			Transform* transform = mPlayer.GetComponent<Transform>();
			transform->position = Math::AddVector(transform->position, Math::ScaleVector(velocity, deltaTime));
		}

		// 플레이어 상태를 업데이트한다.
		{
			Animator* animator = mPlayer.GetComponent<Animator>();

			switch (mPlayerState)
			{
			case PlayerState::Idle:
				animator->SetClip(&mPlayerClips[uint32_t(PlayerState::Idle)]);
				break;

			case PlayerState::Run:
				animator->SetClip(&mPlayerClips[uint32_t(PlayerState::Run)]);
				break;

			case PlayerState::Count:
				break;

			default:
				break;
			}
		}
	}

	return true;
}

void MainScene::Finalize()
{
	mFont.Finalize();
	mMonsterTexture.Finalize();

	for (Texture& texture : mPlayerIdleTextures)
	{
		texture.Finalize();
	}

	for (Texture& texture : mPlayerRunTextures)
	{
		texture.Finalize();
	}
}
