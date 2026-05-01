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

		// Gun
		mGunTexture.Initialize(GetHelper(), "Resource/Gun/0.png");

		// Bullet
		{
			for (uint32_t i = 0; i < Effect::BULLET_COUNT; ++i)
			{
				mBulletTextures[i].Initialize(GetHelper(), "Resource/Gun/Bullet/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mBulletTextures[i];
				frame.durationTime = 0.02f;

				mBulletClip.AddClip(frame);
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
		mSword.offset = PLAYER_HAND;

		Transform transform{};
		transform.scale = { .width = 3.0f, .height = 3.0f };
		transform.center = { .x = 0.0f,.y = 1.0f };
		mSwordEntity.AddComponent(transform);

		mSwordClip.SetLoop(true);

		Animator animator{};
		animator.clipState = &mSwordClip;
		animator.active = true;
		mSwordEntity.AddComponent(animator);

		GetEntityWorld()->AddEntity(&mSwordEntity);
	}

	// Gun
	{
		mGun.offset = { .x = -70.0f, .y = 5.0f };

		Transform transform{};
		transform.scale = { .width = 3.0f, .height = 3.0f };
		transform.center = { .x = 0.0f,.y = 1.0f };
		mGunEntity.AddComponent(transform);

		Image material;
		material.texture = &mGunTexture;
		material.active = true;
		mGunEntity.AddComponent(material);

		GetEntityWorld()->AddEntity(&mGunEntity);
	}

	// Bullet
	{
		Transform transform{};
		transform.scale = { .width = 2.0f, .height = 2.0f };
		mBulletEntity.AddComponent(transform);

		mBulletClip.SetLoop(true);

		Animator animator{};
		animator.clipState = &mBulletClip;
		mBulletEntity.AddComponent(animator);

		GetEntityWorld()->AddEntity(&mBulletEntity);
	}

	// Monster
	{
		Transform transform{};
		transform.position = { .x = 0.0f, .y = 500.0f };
		transform.scale = { .width = 4.0f, .height = 4.0f };
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
		transform->position = target->position;
	}

	// Sword
	{
		constexpr float PLAYER_RADIUS = 13.0f;

		const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();
		Transform* swordTransform = mSwordEntity.GetComponent<Transform>();

		Animator* anim = mSwordEntity.GetComponent<Animator>();

		if (not mSword.isFlying)
		{
			anim->active = true;

			Point mouseToSword = getWorldMousePosition() - swordTransform->position;
			mouseToSword.y *= -1.0f;
			const float degree = std::atan2(mouseToSword.x, mouseToSword.y) * (180.0f / 3.141592f);
			swordTransform->angle = degree;

			const Point mouseToPlayer = getWorldMousePosition() - playerTransform->position;
			const float length = Math::GetVectorLength(mouseToPlayer);

			mSword.direction = mouseToPlayer / length;

			swordTransform->position = mSword.direction * PLAYER_RADIUS * 3.141592f;

			mSword.coolTime += deltaTime;
			if (mSword.coolTime >= Sword::COOLTIMER)
			{
				mSword.isFlying = true;
				mSword.coolTime = 0.0f;
			}
		}

		Point velocity = mSword.direction * Sword::SPEED;
		swordTransform->position = swordTransform->position + velocity * deltaTime;

		const Point toSword =
		{
			.x = swordTransform->position.x - playerTransform->position.x,
			.y = swordTransform->position.y - playerTransform->position.y,
		};

		const float distSq = (toSword.x * toSword.x) + (toSword.y * toSword.y);
		const float rangeSq = Sword::LENGTH * Sword::LENGTH;

		if (distSq >= rangeSq)
		{
			mSword.isFlying = false;
			anim->active = false;
			swordTransform->position = mSword.direction * PLAYER_RADIUS * 3.141592f;
		}

		swordTransform->flip = (mSword.direction.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	}

	// Gun
	{
		const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();
		const bool isRight = getWorldMousePosition().x > playerTransform->position.x;

		if (isRight)
		{
			mGun.directionX = 1.0f;
			mGun.offset = { .x = 10.0f, .y = 5.0f };
		}
		else
		{
			mGun.directionX = -1.0f;
			mGun.offset = { .x = -70.0f, .y = 5.0f };
		}

		constexpr float PLAYER_RADIUS = 20.0f;
		Transform* transform = mGunEntity.GetComponent<Transform>();
		transform->position = mSword.direction * PLAYER_RADIUS * 3.141592f;

		transform->flip = (mGun.directionX > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	}

	// Bullet
	{
		const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();
		const bool isRight = getWorldMousePosition().x > playerTransform->position.x;
		mBullet.offset.x = (isRight) ? 10.0f : -70.0f;

		Transform* bulletTransform = mBulletEntity.GetComponent<Transform>();
		Animator* anim = mBulletEntity.GetComponent<Animator>();

		mBullet.coolTime += deltaTime;
		if (mBullet.coolTime >= Bullet::COOLTIMER)
		{
			anim->frameIndex = 0;
			anim->elapsedTime = 0.0f;

			anim->active = true;
			bulletTransform->position = playerTransform->position;

			const Point difference = getWorldMousePosition() - playerTransform->position;
			const float length = Math::GetVectorLength(difference);

			mBullet.direction =
			{
				.x = difference.x / length,
				.y = difference.y / length
			};

			mBullet.coolTime = 0.0f;
		}

		Point velocity = mBullet.direction * Bullet::SPEED;
		bulletTransform->position = bulletTransform->position + velocity * deltaTime;

		const Point toBullet =
		{
			.x = bulletTransform->position.x - playerTransform->position.x,
			.y = bulletTransform->position.y - playerTransform->position.y,
		};

		const float distSq = (toBullet.x * toBullet.x) + (toBullet.y * toBullet.y);
		const float rangeSq = Bullet::LENGTH * Bullet::LENGTH;
		
		if (distSq >= rangeSq)
		{
			anim->active = false;
		}
	}

	// 몬스터를 업데이트한다.
	{
		// State
		{
			const Transform* monsterTransform = mMonsterEntity.GetComponent<Transform>();
			const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();

			const Point monsterPosition = monsterTransform->position;
			const Point playerPosition = playerTransform->position;

			mMonster.difference = playerPosition - monsterPosition;
			mMonster.length = Math::GetVectorLength(mMonster.difference);

			constexpr float RUN_DISTANCE = 400.0f;
			constexpr float ATTACK_DISTANCE = 90.0f;

			if (mMonster.length <= ATTACK_DISTANCE)
			{
				mMonster.state = Monster::State::Attack;
			}
			else if (mMonster.length <= RUN_DISTANCE)
			{
				mMonster.state = Monster::State::Run;
			}
			else
			{
				mMonster.state = Monster::State::Idle;
			}
		}

		// Move
		{
			Transform* transfrom = mMonsterEntity.GetComponent<Transform>();
			Point position = transfrom->position;

			constexpr float MAX_SPEED = 300.0f;
			Point velocity = {};

			if (mMonster.state == Monster::State::Run)
			{
				if (mMonster.length > 0.0f)
				{
					mMonster.direction =
					{
						.x = mMonster.difference.x / mMonster.length,
						.y = mMonster.difference.y / mMonster.length
					};

					velocity = mMonster.direction * MAX_SPEED;
				}
			}

			transfrom->position = transfrom->position + velocity * deltaTime;
			transfrom->flip = (mMonster.direction.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		}

		// Set Clip
		{
			Animator* animator = mMonsterEntity.GetComponent<Animator>();

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
	const int32_t moveY = Input::Get().GetKey(SDL_SCANCODE_W) - Input::Get().GetKey(SDL_SCANCODE_S);

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

Point MainScene::getWeaponPosition(Transform* swordTransform, Transform* playerTransform, float playerRadius)
{
	assert(swordTransform != nullptr);
	assert(playerTransform != nullptr);

	Point mouseToSword = getWorldMousePosition() - swordTransform->position;
	mouseToSword.y *= -1.0f;
	const float degree = std::atan2(mouseToSword.x, mouseToSword.y) * (180.0f / 3.141592f);
	swordTransform->angle = degree;

	const Point mouseToPlayer = getWorldMousePosition() - playerTransform->position;
	const float length = Math::GetVectorLength(mouseToPlayer);

	mSword.direction = mouseToPlayer / length;

	swordTransform->position = mSword.direction * playerRadius * 3.141592f;

	return swordTransform->position;
}