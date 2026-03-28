#include "pch.h"
#include "Core/Entity/EntityWorld.h"

#include "Core/Constant.h"
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
			for (uint32_t i = 0; i < Player::IDLE_COUNT; ++i)
			{
				mPlayerIdleTextures[i].Initialize(GetHelper(), "Resource/Char/Alice/Idle/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mPlayerIdleTextures[i];
				frame.durationTime = 0.12f;

				mPlayerClips[uint32_t(Player::State::Idle)].AddClip(frame);
			}

			for (uint32_t i = 0; i < Player::RUN_COUNT; ++i)
			{
				mPlayerRunTextures[i].Initialize(GetHelper(), "Resource/Char/Alice/Run/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mPlayerRunTextures[i];
				frame.durationTime = 0.12f;

				mPlayerClips[uint32_t(Player::State::Run)].AddClip(frame);
			}
		}

		// Sword
		{
			for (uint32_t i = 0; i < Sword::COUNT; ++i)
			{
				mSwordTextures[i].Initialize(GetHelper(), "Resource/Sword/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mSwordTextures[i];
				frame.durationTime = 0.12f;

				mSwordClip.AddClip(frame);
			}
		}

		// Monster
		{
			{
				mMonsterIdleTexture.Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Idle/0.png");

				Clip::Frame frame;
				frame.texture = &mMonsterIdleTexture;
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(Monster::State::Idle)].AddClip(frame);
			}

			for (uint32_t i = 0; i < Monster::RUN_COUNT; ++i)
			{
				mMonsterRunTextures[i].Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Run/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mMonsterRunTextures[i];
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(Monster::State::Run)].AddClip(frame);
			}

			for (uint32_t i = 0; i < Monster::ATTACK_COUNT; ++i)
			{
				mMonsterAttackTextures[i].Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Attack/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mMonsterAttackTextures[i];
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(Monster::State::Attack)].AddClip(frame);
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
		label.active = true;
		label.SetText(GetHelper(), "UI Label");
		mLabel.AddComponent(label);

		GetEntityWorld()->AddEntity(&mLabel);
	}

	// Player
	{
		Transform transform{};
		transform.scale = { .width = 4.0f, .height = 4.0f };
		mPlayerEntity.AddComponent(transform);

		mPlayerClips[uint32_t(Player::State::Idle)].SetLoop(true);
		mPlayerClips[uint32_t(Player::State::Run)].SetLoop(true);

		Animator animator{};
		animator.clipState = &mPlayerClips[uint32_t(Player::State::Idle)];
		animator.active = true;
		mPlayerEntity.AddComponent(animator);

		GetEntityWorld()->AddEntity(&mPlayerEntity);
	}

	// Sword
	{
		mSword.offset = { .x = 25.0f, .y = -100.0f };
		mSword.swingTime = 0.25f;
		mSword.coolTime = 0.8f;

		Transform transform{};
		transform.scale = { .width = 3.0f, .height = 3.0f };
		transform.center = 
		{ 
			.x = float(mSwordTextures[0].GetWidth()) * 0.5f * transform.scale.width, 
			.y = float(mSwordTextures[0].GetHeight()) * transform.scale.height 
		};
		mSwordEntity.AddComponent(transform);
		
		mSwordClip.SetLoop(true);

		Animator animator{};
		animator.clipState = &mSwordClip;
		animator.active = true;
		mSwordEntity.AddComponent(animator);

		GetEntityWorld()->AddEntity(&mSwordEntity);
	}

	// Monster
	{
		Transform transform{};
		transform.position = { .x = 0.0f, .y = -500.0f };
		transform.scale = { .width = 3.0f, .height = 3.0f };
		mMonsterEntity.AddComponent(transform);

		mMonsterClips[uint32_t(Monster::State::Idle)].SetLoop(true);
		mMonsterClips[uint32_t(Monster::State::Run)].SetLoop(true);
		mMonsterClips[uint32_t(Monster::State::Attack)].SetLoop(true);

		Animator animator{};
		animator.clipState = &mMonsterClips[uint32_t(Monster::State::Idle)];
		animator.active = true;
		mMonsterEntity.AddComponent(animator);

		GetEntityWorld()->AddEntity(&mMonsterEntity);
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
		Transform* target = mPlayerEntity.GetComponent<Transform>();
		constexpr Point OFFSET = { .x = 30.0f, .y = 10.0f };

		transform->position = target->position + OFFSET;
	}

	// Sword
	{
		Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();
		Transform* swordTransform = mSwordEntity.GetComponent<Transform>();
		
		swordTransform->position = playerTransform->position + mSword.offset;

		bool isRight = getWorldMousePosition().x > playerTransform->position.x;
		mSword.dir = (isRight) ? 1.0f : -1.0f;

		mSword.coolTimer += deltaTime;
		if (not mSword.isSwinging)
		{
			swordTransform->angle = mSword.dir * mSword.INIT_ANGLE;

			if (mSword.coolTimer >= mSword.coolTime)
			{
				mSword.isSwinging = true;
				mSword.coolTimer = 0.0f;
			}
		}
		else
		{
			float t = mSword.coolTimer / mSword.swingTime;

			if (t >= 1.0f)
			{
				t = 1.0f;
				mSword.isSwinging = false;
				mSword.coolTimer = 0.0f;
			}

			swordTransform->angle = mSword.dir * mSword.MAX_ANGLE * t;
		}
	}

	// 몬스터를 업데이트한다.
	{
		const Transform* monsterTransform = mMonsterEntity.GetComponent<Transform>();
		const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();

		Point monsterPosition = monsterTransform->position;
		Point playerPosition = playerTransform->position;

		Point distanceSqrt = playerPosition - monsterPosition;
		const float length = Math::GetVectorLength(distanceSqrt);

		constexpr float RUN_DISTANCE = 200.0f;
		constexpr float ATTACK_DISTANCE = 90.0f;

		Animator* animator = mMonsterEntity.GetComponent<Animator>();

		if (length <= ATTACK_DISTANCE)
		{
			mMonster.state = Monster::State::Attack;
		}
		else if (length <= RUN_DISTANCE)
		{
			mMonster.state = Monster::State::Run;
		}
		else
		{
			mMonster.state = Monster::State::Idle;
		}

		switch (mMonster.state)
		{
			case Monster::State::Idle:
				animator->SetClip(&mMonsterClips[uint32_t(Monster::State::Idle)]);
				break;

			case Monster::State::Run:
				animator->SetClip(&mMonsterClips[uint32_t(Monster::State::Run)]);
				break;

			case Monster::State::Attack:
				animator->SetClip(&mMonsterClips[uint32_t(Monster::State::Attack)]);
				break;

			default:
				assert(false and "지원하지 않는 애니메이션입니다.");
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

	for (Texture& texture : mSwordTextures)
	{
		texture.Finalize();
	}
}

void MainScene::Input()
{
	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
	{
		mIsUpdate = false;
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
	mPlayer.state = (mPlayer.length != 0.0f) ? Player::State::Run : Player::State::Idle;
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

	mPlayer.length = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (mPlayer.length > 0.0f)
	{
		mPlayer.direction = { .x = velocity.x / mPlayer.length, .y = velocity.y / mPlayer.length };

		velocity = mPlayer.direction * MAX_SPEED;
	}

	Transform* transform = mPlayerEntity.GetComponent<Transform>();
	transform->position = transform->position + velocity * deltaTime;
	transform->flip = (mPlayer.direction.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}

void MainScene::SetClip()
{
	Animator* animator = mPlayerEntity.GetComponent<Animator>();
	
	switch (mPlayer.state)
	{
	case Player::State::Idle:
		animator->SetClip(&mPlayerClips[uint32_t(Player::State::Idle)]);
		break;

	case Player::State::Run:
		animator->SetClip(&mPlayerClips[uint32_t(Player::State::Run)]);
		break;

	default:
		assert(false);
		break;
	}
}

Point MainScene::getWorldMousePosition() const
{
	const Point centerOffset =
	{
		.x = (Constant::Get().GetWidth() - 1.0f) * 0.5f,
		.y = (Constant::Get().GetHeight() - 1.0f) * 0.5f,
	};

	Point mousePosition = Input::Get().GetMousePosition();
	mousePosition = mousePosition + mMainCamera.GetComponent<Transform>()->position;
	mousePosition = mousePosition - centerOffset;

	return mousePosition;
}
