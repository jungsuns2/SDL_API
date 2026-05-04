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

		// Tile
		mTileTextures[0].Initialize(GetHelper(), "Resource/Tile/0.png");
		mTileTextures[1].Initialize(GetHelper(), "Resource/Tile/1.png");

		// Player
		{
			for (uint32_t i = 0; i < Player::IDLE_COUNT; ++i)
			{
				mPlayerIdleTextures[i].Initialize(GetHelper(), "Resource/Char/Alice/Idle/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mPlayerIdleTextures[i];
				frame.durationTime = 0.12f;

				mPlayerClips[uint32_t(Player::eState::Idle)].AddClip(frame);
			}

			for (uint32_t i = 0; i < Player::RUN_COUNT; ++i)
			{
				mPlayerRunTextures[i].Initialize(GetHelper(), "Resource/Char/Alice/Run/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mPlayerRunTextures[i];
				frame.durationTime = 0.12f;

				mPlayerClips[uint32_t(Player::eState::Run)].AddClip(frame);
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
			// Spwan
			{
				mMonsterSpwanTexture.Initialize(GetHelper(), "Resource/Monster/Effect/Die/Die01.png");

				Clip::Frame frame;
				frame.texture = &mMonsterSpwanTexture;
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(Monster::eState::Spwan)].AddClip(frame);
			}

			// Idle
			{
				mMonsterIdleTexture.Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Idle/0.png");

				Clip::Frame frame;
				frame.texture = &mMonsterIdleTexture;
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(Monster::eState::Idle)].AddClip(frame);
			}

			// Run
			for (uint32_t i = 0; i < Monster::RUN_COUNT; ++i)
			{
				mMonsterRunTextures[i].Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Run/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mMonsterRunTextures[i];
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(Monster::eState::Run)].AddClip(frame);
			}

			// Attack
			for (uint32_t i = 0; i < Monster::ATTACK_COUNT; ++i)
			{
				mMonsterAttackTextures[i].Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Attack/" + std::to_string(i) + ".png");

				Clip::Frame frame;
				frame.texture = &mMonsterAttackTextures[i];
				frame.durationTime = 0.12f;

				mMonsterClips[uint32_t(Monster::eState::Attack)].AddClip(frame);
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

	// Tile
	{
		constexpr uint32_t TILE_SIZE = 16;
		constexpr float TILE_SCALE = 4.0f;

		FILE* file = nullptr;
		fopen_s(&file, "Resource/Tile/Tile.txt", "r");
		assert(file != nullptr);

		uint32_t width{};
		uint32_t height{};

		fscanf_s(file, "%d %d", &width, &height);

		mTileWidth = width;
		mTileHeight = height;
		mTileEntities = new Entity * [height];

		for (uint32_t y = 0; y < height; ++y)
		{
			const float offsetY = float(height) * float(TILE_SIZE - 1);

			mTileEntities[y] = new Entity[width];

			for (uint32_t x = 0; x < width; ++x)
			{
				uint32_t tileIndex = 0;
				fscanf_s(file, "%d", &tileIndex);
				assert(mTileTextures.size() > tileIndex and "지원하지 않는 타일입니다.");

				Texture& tileTexture = mTileTextures[tileIndex];
				assert(tileTexture.GetWidth() == TILE_SIZE and tileTexture.GetHeight() == TILE_SIZE and "지원하지 않는 타일 사이즈입니다.");

				Entity& tile = mTileEntities[y][x];

				Transform transform{};
				transform.scale = { .width = TILE_SCALE, .height = TILE_SCALE };
				transform.position = { .x = x * TILE_SIZE * TILE_SCALE, .y = (offsetY - y * TILE_SIZE) * TILE_SCALE };
				tile.AddComponent(transform);

				Image image{};
				image.texture = &tileTexture;
				image.active = true;
				tile.AddComponent(image);
				
				Color color{};
				color.r = 255;
				color.g = 255;
				color.b = 255;
				color.a = 255;
				tile.AddComponent(color);

				GetEntityWorld()->AddEntity(&tile);
			}
		}

		fclose(file);
	}

	// Label
	{
		Transform transform;
		transform.position = {};
		mLabelEntity.AddComponent(transform);

		Label label;
		label.font = &mFont;
		label.active = true;
		label.SetText(GetHelper(), "UI Label");
		mLabelEntity.AddComponent(label);

		Color color{};
		color.r = 255;
		color.g = 255;
		color.b = 255;
		color.a = 255;
		mLabelEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mLabelEntity);
	}

	// Player
	{
		Transform transform{};
		transform.scale = { .width = 4.0f, .height = 4.0f };
		mPlayerEntity.AddComponent(transform);

		mPlayerClips[uint32_t(Player::eState::Idle)].SetLoop(true);
		mPlayerClips[uint32_t(Player::eState::Run)].SetLoop(true);

		Animator animator{};
		animator.clipState = &mPlayerClips[uint32_t(Player::eState::Idle)];
		animator.active = true;
		mPlayerEntity.AddComponent(animator);

		Color color{};
		color.r = 255;
		color.g = 255;
		color.b = 255;
		color.a = 255;
		mPlayerEntity.AddComponent(color);

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

		Color color{};
		color.r = 255;
		color.g = 255;
		color.b = 255;
		color.a = 255;
		mSwordEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mSwordEntity);
	}

	// Gun
	{
		mGun.offset = { .x = -70.0f, .y = 5.0f };

		Transform transform{};
		transform.scale = { .width = 3.0f, .height = 3.0f };
		transform.center = { .x = 0.0f,.y = 0.5f };
		mGunEntity.AddComponent(transform);

		Image image;
		image.texture = &mGunTexture;
		image.active = true;
		mGunEntity.AddComponent(image);

		Color color{};
		color.r = 255;
		color.g = 255;
		color.b = 255;
		color.a = 255;
		mGunEntity.AddComponent(color);

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

		Color color{};
		color.r = 255;
		color.g = 255;
		color.b = 255;
		color.a = 255;
		mBulletEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mBulletEntity);
	}

	// Monster
	{
		mMonster.state = Monster::eState::Dead;
		mMonster.hp = 10;

		Transform transform{};
		transform.position = { .x = 0.0f, .y = 300.0f };
		transform.scale = { .width = 4.0f, .height = 4.0f };
		mMonsterEntity.AddComponent(transform);

		mMonsterClips[uint32_t(Monster::eState::Spwan)].SetLoop(true);
		mMonsterClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mMonsterClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		Animator animator{};
		animator.clipState = &mMonsterClips[uint32_t(Monster::eState::Dead)];
		mMonsterEntity.AddComponent(animator);

		Color color{};
		color.r = 255;
		color.g = 255;
		color.b = 255;
		color.a = 255;
		mMonsterEntity.AddComponent(color);

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

	// 칼을 업데이트한다.
	{
		constexpr float PLAYER_RADIUS = 13.0f;

		const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();
		Transform* swordTransform = mSwordEntity.GetComponent<Transform>();

		Animator* anim = mSwordEntity.GetComponent<Animator>();

		if (not mSword.isFlying)
		{
			anim->active = true;

			const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;
			float degree = std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);
			degree -= 90.0f;
			swordTransform->angle = -degree;

			const float length = Math::GetVectorLength(mouseToPlayer);

			mSword.direction = mouseToPlayer / length;

			swordTransform->position = mSword.direction * PLAYER_RADIUS * 3.141592f + playerTransform->position;

			mSword.coolTimer += deltaTime;
			if (mSword.coolTimer >= Sword::COOLTIME)
			{
				mSword.isFlying = true;
				mSword.coolTimer = 0.0f;
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

	// 총을 업데이트한다.
	{
		const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();
		const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;
		float degree = std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);

		Transform* gunTransform = mGunEntity.GetComponent<Transform>();
		gunTransform->angle = -degree;

		const float length = Math::GetVectorLength(mouseToPlayer);
		mGun.direction = mouseToPlayer / length;

		constexpr float PLAYER_RADIUS = 20.0f;
		gunTransform->position = mGun.direction * PLAYER_RADIUS * 3.141592f + playerTransform->position;

		gunTransform->flip = (mGun.direction.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;
	}

	// 총알을 업데이트한다.
	{
		const Transform* gunTransform = mGunEntity.GetComponent<Transform>();

		Transform* bulletTransform = mBulletEntity.GetComponent<Transform>();
		Animator* anim = mBulletEntity.GetComponent<Animator>();

		mBullet.coolTimer += deltaTime;
		if (mBullet.coolTimer >= Bullet::COOLTIME)
		{
			anim->frameIndex = 0;
			anim->elapsedTime = 0.0f;

			anim->active = true;
			bulletTransform->position = gunTransform->position;

			const Point difference = getScreenMousePosition() - gunTransform->position;
			const float length = Math::GetVectorLength(difference);

			mBullet.direction =
			{
				.x = difference.x / length,
				.y = difference.y / length
			};

			mBullet.coolTimer = 0.0f;
		}

		Point velocity = mBullet.direction * Bullet::SPEED;
		bulletTransform->position = bulletTransform->position + velocity * deltaTime;

		const Point toBullet =
		{
			.x = bulletTransform->position.x - gunTransform->position.x,
			.y = bulletTransform->position.y - gunTransform->position.y,
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

			constexpr float ATTACK_DISTANCE = 90.0f;
			Animator* anim = mMonsterEntity.GetComponent<Animator>();

			if (not anim->active
				and not mMonster.isSpwan)
			{
				mMonster.spwanPositionTimer += deltaTime;
				if (mMonster.spwanPositionTimer >= 2.0f)
				{
					mMonster.state = Monster::eState::Spwan;
					mMonster.isSpwan = true;
					anim->active = true;

					mMonster.spwanWaitingTimer = 0.0f;
					mMonster.spwanPositionTimer = 0.0f;
				}
			}

			if (mMonster.state == Monster::eState::Spwan)
			{
				mMonster.spwanWaitingTimer += deltaTime;
				if (mMonster.spwanWaitingTimer >= 0.5f)
				{

					mMonster.spwanBlinkTimer += deltaTime;
					if (mMonster.spwanBlinkTimer >= 0.06f)
					{
						anim->active = !anim->active;
						mMonster.spwanBlinkTimer = 0.0f;
					}
				}
				if (mMonster.spwanWaitingTimer >= 1.0f)
				{
					mMonster.state = Monster::eState::Run;
					anim->active = true;
					mMonster.spwanBlinkTimer = 0.0f;
					mMonster.spwanWaitingTimer = 0.0f;
				}
			}

			Clip& attackClip = mMonsterClips[uint32_t(Monster::eState::Attack)];
			if (mMonster.state == Monster::eState::Attack)
			{
				if (anim->clipState == &attackClip
					and anim->frameIndex >= attackClip.GetLastFrameIndex() - 1)
				{
					if (mMonster.length > ATTACK_DISTANCE)
					{
						mMonster.state = Monster::eState::Run;
					}
				}
			}
			else if (mMonster.state == Monster::eState::Run)
			{
				if (mMonster.length <= ATTACK_DISTANCE)
				{
					mMonster.state = Monster::eState::Attack;
				}
			}

			// 충돌했을 때 애니메이션 처리
			if (mMonster.state != Monster::eState::Spwan
				and mMonster.state != Monster::eState::Dead)
			{
				if (Input::Get().GetKeyDown(SDL_SCANCODE_T))
				{
					mMonster.hp -= 1;
				}
			}

			static int32_t prevHp = mMonster.hp;
			if (prevHp != mMonster.hp)
			{
				Color* color = mMonsterEntity.GetComponent<Color>();
				color->r = 240;
				color->g = 0;
				color->b = 0;

				mMonster.damageTimer += deltaTime;
				if (mMonster.damageTimer >= Monster::DAMAGE_TIME)
				{
					mMonster.state = Monster::eState::Run;
					Color* color = mMonsterEntity.GetComponent<Color>();
					color->r = 255;
					color->g = 255;
					color->b = 255;

					prevHp = mMonster.hp;
					mMonster.damageTimer = 0.0f;
				}
			}

			if (mMonster.hp <= 0)
			{
				mMonster.state = Monster::eState::Dead;
				Animator* anim = mMonsterEntity.GetComponent<Animator>();
				anim->active = false;

				mMonster.deadTimer += deltaTime;
				if (mMonster.deadTimer >= Monster::DEAD_TIME)
				{
					mMonster.hp = 10;
					mMonster.isSpwan = false;
					mMonster.deadTimer = 0.0f;
				}
			}
		}

		// Move
		{
			Transform* transfrom = mMonsterEntity.GetComponent<Transform>();
			Point position = transfrom->position;

			constexpr float MAX_SPEED = 300.0f;
			Point velocity = {};

			if (mMonster.state == Monster::eState::Run)
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
			case Monster::eState::Spwan:
				animator->SetClip(&mMonsterClips[uint32_t(Monster::eState::Spwan)]);
				break;

			case Monster::eState::Idle:
				animator->SetClip(&mMonsterClips[uint32_t(Monster::eState::Idle)]);
				break;

			case Monster::eState::Run:
				animator->SetClip(&mMonsterClips[uint32_t(Monster::eState::Run)]);
				break;

			case Monster::eState::Attack:
				animator->SetClip(&mMonsterClips[uint32_t(Monster::eState::Attack)]);
				break;

			case Monster::eState::Dead:
				animator->SetClip(&mMonsterClips[uint32_t(Monster::eState::Spwan)]);
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
		mMonsterSpwanTexture.Finalize();
		mGunTexture.Finalize();

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

	for (Texture& texture : mTileTextures)
	{
		texture.Finalize();
	}

	for (Texture& texture : mBulletTextures)
	{
		texture.Finalize();
	}

	{
		for (uint32_t y = 0; y < mTileHeight; ++y)
		{
			delete[] mTileEntities[y];
		}

		delete[] mTileEntities;
	}
}

void MainScene::Input()
{
	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
	{
		mIsUpdate = false;
	}
}

void MainScene::State()
{
	mPlayer.state = (mPlayer.length != 0.0f) ? Player::eState::Run : Player::eState::Idle;
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
	case Player::eState::Idle:
		animator->SetClip(&mPlayerClips[uint32_t(Player::eState::Idle)]);
		break;

	case Player::eState::Run:
		animator->SetClip(&mPlayerClips[uint32_t(Player::eState::Run)]);
		break;

	default:
		assert(false);
		break;
	}
}

Point MainScene::getScreenMousePosition() const
{
	const Point centerOffset =
	{
		.x = (Constant::Get().GetWidth() - 1.0f) * 0.5f,
		.y = (Constant::Get().GetHeight() - 1.0f) * 0.5f
	};

	const Point mousePosition = Input::Get().GetMousePosition();
	Point screenPosition =
	{
		.x = mousePosition.x - centerOffset.x,
		.y = centerOffset.y - mousePosition.y
	};
	
	screenPosition = screenPosition + mMainCamera.GetComponent<Transform>()->position;

	return screenPosition;
}

Point MainScene::getWeaponPosition(Transform* swordTransform, Transform* playerTransform, float playerRadius)
{
	assert(swordTransform != nullptr);
	assert(playerTransform != nullptr);

	Point mouseToSword = getScreenMousePosition() - swordTransform->position;
	mouseToSword.y *= -1.0f;
	const float degree = std::atan2(mouseToSword.x, mouseToSword.y) * (180.0f / 3.141592f);
	swordTransform->angle = degree;

	const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;
	const float length = Math::GetVectorLength(mouseToPlayer);

	mSword.direction = mouseToPlayer / length;

	swordTransform->position = mSword.direction * playerRadius * 3.141592f;

	return swordTransform->position;
}