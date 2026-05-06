#include "pch.h"
#include "Core/Entity/EntityWorld.h"

#include "Core/Constant.h"
#include "Core/Helper.h"
#include "Core/Input.h"

#include "MainScene.h"

void MainScene::Initialize()
{
	Initialize_Resource();

	Initialize_Entity();
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
		constexpr float COOLTIME = 1.0f;
		constexpr float LENGTH = 200.0f;
		constexpr float SPEED = 1300.0f;

		const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();
		Transform* swordTransform = mSwordEntity.GetComponent<Transform>();

		Sword* sword = mSwordEntity.GetComponent<Sword>();
		Animator* anim = mSwordEntity.GetComponent<Animator>();
		Active* active = mSwordEntity.GetComponent<Active>();
		Direction* direction = mSwordEntity.GetComponent<Direction>();
		WeaponState* swordState = mSwordEntity.GetComponent<WeaponState>();

		if (not swordState->isFire)
		{
			active->value = true;

			setWeaponPosition
			(
				{
					.weaponEntity = &mSwordEntity,
					.playerRadius = PLAYER_RADIUS,
					.dgreeOffset = -90.0f,
					.flipX = SDL_FLIP_NONE,
					.flipY = SDL_FLIP_HORIZONTAL
				}
			);

			swordState->fireCoolTimer += deltaTime;
			if (swordState->fireCoolTimer >= COOLTIME)
			{
				swordState->isFire = true;
				swordState->fireCoolTimer = 0.0f;
			}
		}

		Point velocity = direction->point * SPEED;
		swordTransform->position = swordTransform->position + velocity * deltaTime;

		const Point toSword =
		{
			.x = swordTransform->position.x - playerTransform->position.x,
			.y = swordTransform->position.y - playerTransform->position.y,
		};

		const float distSq = (toSword.x * toSword.x) + (toSword.y * toSword.y);
		const float rangeSq = LENGTH * LENGTH;

		if (distSq >= rangeSq)
		{
			swordState->isFire = false;
			active->value = false;
			swordTransform->position = direction->point * PLAYER_RADIUS * 3.141592f;
		}
	}

	// 총을 업데이트한다.
	{
		constexpr float PLAYER_RADIUS = 20.0f;
		setWeaponPosition
		(
			{
				.weaponEntity = &mGunEntity,
				.playerRadius = PLAYER_RADIUS,
				.dgreeOffset = 0.0f,
				.flipX = SDL_FLIP_NONE, 
				.flipY = SDL_FLIP_VERTICAL
			}
		);
	}

	// 총알을 업데이트한다.
	{
		constexpr float FIRE_COOLTIME = 1.0f;
		constexpr float LENGTH = 300.0f;
		constexpr float SPEED = 1300.0f;

		const Transform* gunTransform = mGunEntity.GetComponent<Transform>();

		Bullet* bullet = mBulletEntity.GetComponent<Bullet>();
		Transform* bulletTransform = mBulletEntity.GetComponent<Transform>();
		Direction* bulletDirection = mBulletEntity.GetComponent<Direction>();
		Animator* bulletAnim = mBulletEntity.GetComponent<Animator>();
		Active* active = mBulletEntity.GetComponent<Active>();
		WeaponState* bulletState = mBulletEntity.GetComponent<WeaponState>();

		bulletState->fireCoolTimer += deltaTime;
		if (bulletState->fireCoolTimer >= FIRE_COOLTIME)
		{
			bulletAnim->frameIndex = 0;
			bulletAnim->elapsedTime = 0.0f;

			active->value = true;
			bulletTransform->position = gunTransform->position;

			const Point difference = getScreenMousePosition() - gunTransform->position;
			const float length = Math::GetVectorLength(difference);

			bulletDirection->point = difference / length;
			bulletState->fireCoolTimer = 0.0f;
		}

		Point velocity = bulletDirection->point * SPEED;
		bulletTransform->position = bulletTransform->position + velocity * deltaTime;

		const Point toBullet =
		{
			.x = bulletTransform->position.x - gunTransform->position.x,
			.y = bulletTransform->position.y - gunTransform->position.y,
		};

		const float distSq = (toBullet.x * toBullet.x) + (toBullet.y * toBullet.y);
		const float rangeSq = LENGTH * LENGTH;
		
		if (distSq >= rangeSq)
		{
			active->value = false;
		}
	}

	// 몬스터를 업데이트한다.
	{
		// State
		{
			constexpr float SPWAN_SCALE = 2.0f;
			constexpr float ORIGNAL_SCALE = 4.0f;
			constexpr float SPWAN_POSITION_TIME = 2.0f;
			constexpr float SPWAN_WAITING_TIME = 1.0f;
			constexpr float DAMAGE_TIME = 0.3f;
			constexpr float DEAD_TIME = 0.5f;

			Monster* monster = mMonsterEntity.GetComponent<Monster>();
			Transform* monsterTransform = mMonsterEntity.GetComponent<Transform>();
			Active* active = mMonsterEntity.GetComponent<Active>();
			SpwanTimer* spwan = mMonsterEntity.GetComponent<SpwanTimer>();
			

			if (not active->value
				and not spwan->isSpwan)
			{
				spwan->spwanPositionTimer += deltaTime;
				if (spwan->spwanPositionTimer >= SPWAN_POSITION_TIME)
				{
					monster->state = Monster::eState::Spwan;
					spwan->isSpwan = true;
					monsterTransform->scale = { .width = SPWAN_SCALE, .height = SPWAN_SCALE };
					active->value = true;

					spwan->spwanWaitingTimer = 0.0f;
					spwan->spwanPositionTimer = 0.0f;
				}
			}

			if (monster->state == Monster::eState::Spwan)
			{
				spwan->spwanWaitingTimer += deltaTime;

				float waitingTime = SPWAN_WAITING_TIME / 2.0f;
				if (spwan->spwanWaitingTimer >= waitingTime)
				{

					spwan->spwanBlinkTimer += deltaTime;
					if (spwan->spwanBlinkTimer >= 0.06f)
					{
						active->value = !active->value;
						spwan->spwanBlinkTimer = 0.0f;
					}
				}
				if (spwan->spwanWaitingTimer >= SPWAN_WAITING_TIME)
				{
					monsterTransform->scale = { .width = ORIGNAL_SCALE, .height = ORIGNAL_SCALE };

					monster->state = Monster::eState::Run;
					active->value = true;
					spwan->spwanBlinkTimer = 0.0f;
					spwan->spwanWaitingTimer = 0.0f;
				}
			}

			constexpr float ATTACK_DISTANCE = 90.0f;
			Animator* anim = mMonsterEntity.GetComponent<Animator>();
			Clip& attackClip = mMonsterClips[uint32_t(Monster::eState::Attack)];
			if (monster->state == Monster::eState::Attack)
			{
				if (anim->clipState == &attackClip
					and anim->frameIndex >= attackClip.GetLastFrameIndex() - 1)
				{
					if (monster->length > ATTACK_DISTANCE)
					{
						monster->state = Monster::eState::Run;
					}
				}
			}
			else if (monster->state == Monster::eState::Run)
			{
				if (monster->length <= ATTACK_DISTANCE)
				{
					monster->state = Monster::eState::Attack;
				}
			}

			// 충돌했을 때 애니메이션 처리
			Hp* hp = mMonsterEntity.GetComponent<Hp>();

			if (monster->state != Monster::eState::Spwan
				and monster->state != Monster::eState::Dead)
			{
				if (Input::Get().GetKeyDown(SDL_SCANCODE_T))
				{
					hp->value -= 1;
				}
			}

			DamageTimer* damage = mMonsterEntity.GetComponent<DamageTimer>();
			static int32_t prevHp = hp->value;
			if (prevHp != hp->value)
			{
				Color* color = mMonsterEntity.GetComponent<Color>();
				color->r = 200;
				color->g = 0;
				color->b = 0;

				damage->damageTimer += deltaTime;
				if (damage->damageTimer >= DAMAGE_TIME)
				{
					monster->state = Monster::eState::Run;
					Color* color = mMonsterEntity.GetComponent<Color>();
					color->r = 255;
					color->g = 255;
					color->b = 255;

					prevHp = hp->value;
					damage->damageTimer = 0.0f;
				}
			}

			// 파티클이 생성된다.
			constexpr float SPEED = 300.0f;
			if (hp->value <= 0)
			{
				if (monster->state != Monster::eState::Dead
					and monster->state != Monster::eState::Spwan)
				{
					for (Entity& entity : mDeadParticleEntities)
					{
						Particle* particle = entity.GetComponent<Particle>();
						Direction* direction = entity.GetComponent<Direction>();
						Point& dir = direction->point;

						dir = getScreenMousePosition() - monsterTransform->position;
						dir = Math::NormalizeVector(dir);
						dir = Math::RotatePoint(dir, getRandom(-60.0f, 60.0f));

						Transform* transform = entity.GetComponent<Transform>();
						transform->position = monsterTransform->position;
					}
				}

				monster->state = Monster::eState::Dead;
				Active* monsterActive = mMonsterEntity.GetComponent<Active>();
				monsterActive->value = false;

				for (Entity& entity : mDeadParticleEntities)
				{
					if (not monsterActive->value)
					{
						Active* particleActive = entity.GetComponent<Active>();
						particleActive->value = true;

						Transform* transform = entity.GetComponent<Transform>();
						Particle* particle = entity.GetComponent<Particle>();
						Direction* direction = entity.GetComponent<Direction>();
						transform->position = transform->position + direction->point * SPEED * deltaTime;
					}
				}

				damage->deadTimer += deltaTime;
				if (damage->deadTimer >= DEAD_TIME)
				{
					for (Entity& entity : mDeadParticleEntities)
					{
						Active* active = entity.GetComponent<Active>();
						active->value = false;
					}

					hp->value = hp->max;
					spwan->isSpwan = false;
					damage->deadTimer = 0.0f;
				}
			}
		}

		// Move
		{
			constexpr float MAX_SPEED = 300.0f;
			Point velocity = {};
			
			Monster* monster = mMonsterEntity.GetComponent<Monster>();
			Transform* monsterTransform = mMonsterEntity.GetComponent<Transform>();
			const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();

			const Point monsterPosition = monsterTransform->position;
			const Point playerPosition = playerTransform->position;

			const Point difference = playerPosition - monsterPosition;
			monster->length = Math::GetVectorLength(difference);

			Direction* direction = mMonsterEntity.GetComponent<Direction>();

			if (monster->state == Monster::eState::Run)
			{
				if (monster->length > 0.0f)
				{
					direction->point =
					{
						.x = difference.x / monster->length,
						.y = difference.y / monster->length
					};

					velocity = direction->point * MAX_SPEED;
				}
			}

			monsterTransform->position = monsterTransform->position + velocity * deltaTime;
			monsterTransform->flip = (direction->point.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		}

		// Set Clip
		{
			Animator* animator = mMonsterEntity.GetComponent<Animator>();
			Monster* monster = mMonsterEntity.GetComponent<Monster>();

			switch (monster->state)
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
				animator->SetClip(&mMonsterClips[uint32_t(Monster::eState::Idle)]);
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

	for (Texture& texture : mTileTextures)
	{
		texture.Finalize();
	}

	mGunTexture.Finalize();

	for (Texture& texture : mBulletTextures)
	{
		texture.Finalize();
	}

	// Tile
	{
		for (uint32_t y = 0; y < mTileHeight; ++y)
		{
			delete[] mTileEntities[y];
		}

		delete[] mTileEntities;
	}

	mDeadParticleTexture.Finalize();
}

void MainScene::Initialize_Resource()
{
	mFont.Initilize("Resource/DroidSans.TTF", 50);

	// Tile
	mTileTextures[0].Initialize(GetHelper(), "Resource/Tile/0.png");
	mTileTextures[1].Initialize(GetHelper(), "Resource/Tile/1.png");

	mDeadParticleTexture.Initialize(GetHelper(), "Resource/RedRectangle.png");

	// Player
	{
		uint32_t cnt{};
		for (Texture& texture : mPlayerIdleTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Char/Alice/Idle/" + std::to_string(cnt++) + ".png");

			Clip::Frame frame;
			frame.texture = &texture;
			frame.durationTime = 0.12f;

			mPlayerClips[uint32_t(Player::eState::Idle)].AddClip(frame);
		}

		cnt = 0;
		for (Texture& texture : mPlayerRunTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Char/Alice/Run/" + std::to_string(cnt++) + ".png");

			Clip::Frame frame;
			frame.texture = &texture;
			frame.durationTime = 0.12f;

			mPlayerClips[uint32_t(Player::eState::Run)].AddClip(frame);
		}
	}

	// Sword
	{
		uint32_t cnt{};
		for (Texture& texture : mSwordTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Sword/" + std::to_string(cnt++) + ".png");

			Clip::Frame frame;
			frame.texture = &texture;
			frame.durationTime = 0.12f;

			mSwordClip.AddClip(frame);
		}
	}

	// Gun
	mGunTexture.Initialize(GetHelper(), "Resource/Gun/0.png");

	// Bullet
	{
		uint32_t cnt{};
		for (Texture& texture : mBulletTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Gun/Bullet/" + std::to_string(cnt++) + ".png");

			Clip::Frame frame;
			frame.texture = &texture;
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
		uint32_t cnt{};
		for (Texture& texture : mMonsterRunTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Run/" + std::to_string(cnt++) + ".png");

			Clip::Frame frame;
			frame.texture = &texture;
			frame.durationTime = 0.12f;

			mMonsterClips[uint32_t(Monster::eState::Run)].AddClip(frame);
		}

		cnt = 0;
		for (Texture& texture : mMonsterAttackTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Attack/" + std::to_string(cnt++) + ".png");

			Clip::Frame frame{};
			frame.texture = &texture;
			frame.durationTime = 0.12f;

			mMonsterClips[uint32_t(Monster::eState::Attack)].AddClip(frame);
		}
	}
}

void MainScene::Initialize_Entity()
{
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
				tile.AddComponent(image);

				Active active{};
				active.value = true;
				tile.AddComponent(active);

				Color color{};
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
		mLabelEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mLabelEntity);
	}

	// Player
	{
		const float SIZE = 4.0f;

		Player player{};
		player.state = Player::eState::Idle;
		mPlayerEntity.AddComponent(player);

		Direction direction{};
		mPlayerEntity.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = SIZE, .height = SIZE };
		mPlayerEntity.AddComponent(transform);

		mPlayerClips[uint32_t(Player::eState::Idle)].SetLoop(true);
		mPlayerClips[uint32_t(Player::eState::Run)].SetLoop(true);

		Animator animator{};
		animator.clipState = &mPlayerClips[uint32_t(Player::eState::Idle)];
		mPlayerEntity.AddComponent(animator);

		Active active{};
		active.value = true;
		mPlayerEntity.AddComponent(active);

		Color color{};
		mPlayerEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mPlayerEntity);
	}

	// Sword
	{
		const float SIZE = 3.0f;
		constexpr Point CENTER = { .x = 0.0f,.y = 1.0f };

		Sword sword{};
		mSwordEntity.AddComponent(sword);

		WeaponState state{};
		mSwordEntity.AddComponent(state);

		Direction direction{};
		mSwordEntity.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = SIZE, .height = SIZE };
		transform.center = CENTER;
		mSwordEntity.AddComponent(transform);

		mSwordClip.SetLoop(true);

		Animator animator{};
		animator.clipState = &mSwordClip;
		mSwordEntity.AddComponent(animator);

		Active active{};
		active.value = true;
		mSwordEntity.AddComponent(active);

		Color color{};
		mSwordEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mSwordEntity);
	}

	// Gun
	{
		constexpr float SIZE = 3.0f;
		constexpr Point CENTER = { .x = 0.0f,.y = 0.5f };

		Gun gun{};
		mGunEntity.AddComponent(gun);

		Direction direction{};
		mGunEntity.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = 3.0f, .height = 3.0f };
		transform.center = CENTER;
		mGunEntity.AddComponent(transform);

		Image image;
		image.texture = &mGunTexture;
		mGunEntity.AddComponent(image);

		Active active{};
		active.value = true;
		mGunEntity.AddComponent(active);

		Color color{};
		mGunEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mGunEntity);
	}

	// Bullet
	{
		const float SIZE = 2.0f;

		Bullet bullet{};
		mBulletEntity.AddComponent(bullet);

		WeaponState state{};
		mBulletEntity.AddComponent(state);

		Direction direction{};
		mBulletEntity.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = SIZE, .height = SIZE };
		mBulletEntity.AddComponent(transform);

		mBulletClip.SetLoop(true);

		Animator animator{};
		animator.clipState = &mBulletClip;
		mBulletEntity.AddComponent(animator);

		Active active{};
		mBulletEntity.AddComponent(active);

		Color color{};
		mBulletEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mBulletEntity);
	}

	// Monster
	{
		constexpr uint32_t HP_MAX = 10;
		constexpr Point POSITION = { .x = 0.0f, .y = 300.0f };

		Monster monster{};
		monster.state = Monster::eState::Dead;
		mMonsterEntity.AddComponent(monster);

		Direction direction{};
		mMonsterEntity.AddComponent(direction);

		SpwanTimer spwanTimer{};
		mMonsterEntity.AddComponent(spwanTimer);

		DamageTimer damageTimer{};
		mMonsterEntity.AddComponent(damageTimer);

		Hp hp{};
		hp.max = HP_MAX;
		hp.value = hp.max;
		mMonsterEntity.AddComponent(hp);

		Transform transform{};
		transform.position = POSITION;
		mMonsterEntity.AddComponent(transform);

		mMonsterClips[uint32_t(Monster::eState::Spwan)].SetLoop(true);
		mMonsterClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mMonsterClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		Animator animator{};
		animator.clipState = &mMonsterClips[uint32_t(Monster::eState::Dead)];
		mMonsterEntity.AddComponent(animator);

		Active active{};
		mMonsterEntity.AddComponent(active);

		Color color{};
		mMonsterEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mMonsterEntity);
	}

	// Dead Particle
	{
		constexpr float SIZE = 0.3f;

		for (Entity& entity : mDeadParticleEntities)
		{
			Particle particle{};
			entity.AddComponent(particle);

			Direction direction{};
			entity.AddComponent(direction);

			Transform transform{};
			transform.scale = { .width = SIZE, .height = SIZE };
			entity.AddComponent(transform);

			Image image{};
			image.texture = &mDeadParticleTexture;
			entity.AddComponent(image);

			Active active{};
			entity.AddComponent(active);

			Color color{};
			entity.AddComponent(color);

			GetEntityWorld()->AddEntity(&entity);
		}
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
	Player* player = mPlayerEntity.GetComponent<Player>();
	player->state = (player->length != 0.0f) ? Player::eState::Run : Player::eState::Idle;
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

	Player* player = mPlayerEntity.GetComponent<Player>();
	player->length = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	Direction* direction = mPlayerEntity.GetComponent<Direction>();

	if (player->length > 0.0f)
	{
		direction->point = { .x = velocity.x / player->length, .y = velocity.y / player->length };

		velocity = direction->point * MAX_SPEED;
	}

	Transform* transform = mPlayerEntity.GetComponent<Transform>();
	transform->position = transform->position + velocity * deltaTime;
	transform->flip = (direction->point.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}

void MainScene::SetClip()
{
	Player* player = mPlayerEntity.GetComponent<Player>();
	Animator* animator = mPlayerEntity.GetComponent<Animator>();

	switch (player->state)
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

void MainScene::setWeaponPosition(const SetWeaponDesc& desc)
{
	Entity* weaponEntity = desc.weaponEntity;
	float playerRadius = desc.playerRadius;
	float dgreeOffset = desc.dgreeOffset;
	SDL_RendererFlip flipX = desc.flipX;
	SDL_RendererFlip flipY = desc.flipY;

	Transform* transform = weaponEntity->GetComponent<Transform>();
	Direction* direction = weaponEntity->GetComponent<Direction>();

	const Transform* playerTransform = mPlayerEntity.GetComponent<Transform>();
	const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;
	float degree = std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);
	degree = degree + dgreeOffset;
	transform->angle = -degree;

	const float length = Math::GetVectorLength(mouseToPlayer);
	direction->point = mouseToPlayer / length;

	transform->position = playerTransform->position + direction->point * playerRadius * 3.141592f;
	transform->flip = (direction->point.x > 0.0f) ? flipX : flipY;
}

float MainScene::getRandom(const float min, const float max)
{
	const float result = float(rand()) / RAND_MAX * (max - min) + min;
	return result;
}
