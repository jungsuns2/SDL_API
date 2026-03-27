#include "pch.h"
#include "Core/Entity/EntityWorld.h"

#include "Core/Helper.h"
#include "Core/Input.h"
#include "MainScene.h"

void MainScene::Initialize()
{
	// Resource
	{
		mFont.Initilize("Resource/DroidSans.TTF", 50);

		// Player
		{
			for (uint32_t i = 0; i < PLAYER_IDLE_COUNT; ++i)
			{
				mPlayerIdleTextures[i].Initialize(GetHelper(), "Resource/Char/Alice/Idle/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mPlayerIdleTextures[i];
				frame.durationTime = 0.12f;

				mPlayerClips[uint32_t(PlayerState::Idle)].AddClip(frame);
			}

			for (uint32_t i = 0; i < PLAYER_RUN_COUNT; ++i)
			{
				mPlayerRunTextures[i].Initialize(GetHelper(), "Resource/Char/Alice/Run/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mPlayerRunTextures[i];
				frame.durationTime = 0.12f;

				mPlayerClips[uint32_t(PlayerState::Run)].AddClip(frame);
			}
		}

		// Monster
		{
			{
				mMonsterIdleTexture.Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Idle/0.png");

				Clip::Frame frame;
				frame.texture = &mMonsterIdleTexture;
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(MonsterState::Idle)].AddClip(frame);
			}

			for (uint32_t i = 0; i < MONSTER_RUN_COUNT; ++i)
			{
				mMonsterRunTextures[i].Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Run/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mMonsterRunTextures[i];
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(MonsterState::Run)].AddClip(frame);
			}

			for (uint32_t i = 0; i < MONSTER_ATTACK_COUNT; ++i)
			{
				mMonsterAttackTextures[i].Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Attack/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mMonsterAttackTextures[i];
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(MonsterState::Attack)].AddClip(frame);
			}
		}
	}

	// Camera
	{
		Camera camera{};
		mMainCamera.AddComponent(camera);

		Transform transform{};
		mMainCamera.AddComponent(transform);

		GetEntityWorld()->AddEntity(&mMainCamera);
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
		Transform transform{};
		transform.scale = { .width = 5.0f, .height = 5.0f };
		mPlayer.AddComponent(transform);

		mPlayerClips[uint32_t(PlayerState::Idle)].SetLoop(true);
		mPlayerClips[uint32_t(PlayerState::Run)].SetLoop(true);

		Animator animator{};
		animator.clipState = &mPlayerClips[uint32_t(PlayerState::Idle)];
		mPlayer.AddComponent(animator);

		GetEntityWorld()->AddEntity(&mPlayer);
	}

	// Monster
	{
		Transform transform;
		transform.position = { .x = 0.0f, .y = -500.0f };
		transform.scale = { .width = 3.0f, .height = 3.0f };
		mMonster.AddComponent(transform);

		mMonsterClips[uint32_t(MonsterState::Idle)].SetLoop(true);
		mMonsterClips[uint32_t(MonsterState::Run)].SetLoop(true);
		mMonsterClips[uint32_t(MonsterState::Attack)].SetLoop(false);

		Animator animator;
		animator.clipState = &mMonsterClips[uint32_t(MonsterState::Idle)];
		animator.elapsedTime = 0.0f;
		mMonster.AddComponent(animator);

		GetEntityWorld()->AddEntity(&mMonster);
	}
}

bool MainScene::Update(const float deltaTime)
{
	// 플레이어를 업데이트한다.
	{
		Input();

		State();

		Move(deltaTime);

		SetClip();
	}

	// 카메라를 업데이트한다.
	{
		Transform* transform = mMainCamera.GetComponent<Transform>();
		Transform* target = mPlayer.GetComponent<Transform>();
		Point offset = { .x = 30.0f, .y = 10.0f };

		transform->position = Math::AddVector(target->position, offset);
	}

	// 몬스터를 업데이트한다.
	{
		const Transform* monsterTransform = mMonster.GetComponent<Transform>();
		const Transform* playerTransform = mPlayer.GetComponent<Transform>();

		const Point monsterPosition = monsterTransform->position;
		const Point playerPosition = playerTransform->position;

		Point distanceSqrt = Math::SubtractVector(playerPosition, monsterPosition);
		const float length = Math::GetVectorLength(distanceSqrt);

		constexpr float RUN_DISTANCE = 200.0f;
		constexpr float ATTACK_DISTANCE = 90.0f;

		if (length <= ATTACK_DISTANCE)
		{
			mMonsterState = MonsterState::Attack;
		}
		else if (length <= RUN_DISTANCE)
		{
			mMonsterState = MonsterState::Run;
		}
		else
		{
			mMonsterState = MonsterState::Idle;
		}

		Animator* animator = mMonster.GetComponent<Animator>();
		switch (mMonsterState)
		{
			case MonsterState::Idle:
				animator->SetClip(&mMonsterClips[uint32_t(MonsterState::Idle)]);
				break;

			case MonsterState::Run:
				animator->SetClip(&mMonsterClips[uint32_t(MonsterState::Run)]);
				break;

			case MonsterState::Attack:
				animator->SetClip(&mMonsterClips[uint32_t(MonsterState::Attack)]);
				break;

			default:
				assert(false && "지원하지 않는 애니메이션입니다.");
				break;
		}
	}

	return mIsUpdate;
}

void MainScene::Finalize()
{
	mFont.Finalize();

	// Player
	{
		for (Texture& texture : mPlayerIdleTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mPlayerRunTextures)
		{
			texture.Finalize();
		}
	}

	// Monster
	{
		mMonsterIdleTexture.Finalize();

		for (Texture& texture : mMonsterRunTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mMonsterAttackTextures)
		{
			texture.Finalize();
		}
	}
}

void MainScene::Input()
{
	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
	{
		mIsUpdate = false;
	}

	if (Input::Get().GetMouseButtonDown(SDL_BUTTON_LEFT))
	{
	}
	if (Input::Get().GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
	}

	static bool mousePositionPrint;

	if (Input::Get().GetKeyDown(SDL_SCANCODE_T))
	{
		mousePositionPrint = !mousePositionPrint;
	}

	if (mousePositionPrint)
	{
	}
}

void MainScene::State()
{
	mPlayerState = (mPlayerLength != 0.0f) ? PlayerState::Run : PlayerState::Idle;
}

void MainScene::Move(const float deltaTime)
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

	mPlayerLength = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (mPlayerLength > 0.0f)
	{
		const Point direction = { .x = velocity.x / mPlayerLength, .y = velocity.y / mPlayerLength };

		velocity = Math::ScaleVector(direction, MAX_SPEED);
	}

	Transform* transform = mPlayer.GetComponent<Transform>();
	transform->position = Math::AddVector(transform->position, Math::ScaleVector(velocity, deltaTime));
}

void MainScene::SetClip()
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
