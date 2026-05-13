#include "pch.h"
#include "Core/Entity/EntityWorld.h"

#include "Core/Constant.h"
#include "Core/Helper.h"
#include "Core/Input.h"

#include "MainScene.h"

void MainScene::Initialize()
{
	initialize_Resource();

	initialize_Entity();
}

bool MainScene::Update(const float deltaTime)
{
	// 플레이어를 업데이트한다.
	{
		input();

		playerState();

		playerMove(deltaTime);

		playerSetClip();
	}

	// 카메라를 업데이트한다.
	{
		Transform* transform = mMainCamera.GetComponent<Transform>();
		Transform* target = mPlayer.GetComponent<Transform>();
		transform->position = target->position;
	}

	// 칼을 업데이트한다.
	{
		constexpr float PLAYER_RADIUS = 13.0f;
		constexpr float COOLTIME = 1.0f;
		constexpr float LENGTH = 200.0f;
		constexpr float SPEED = 1300.0f;

		const Transform* playerTransform = mPlayer.GetComponent<Transform>();
		Transform* swordTransform = mSword.GetComponent<Transform>();

		Sword* sword = mSword.GetComponent<Sword>();
		Animator* anim = mSword.GetComponent<Animator>();
		Active* active = mSword.GetComponent<Active>();
		Direction* direction = mSword.GetComponent<Direction>();
		WeaponState* swordState = mSword.GetComponent<WeaponState>();

		if (not swordState->isFire)
		{
			active->value = true;

			setWeaponPosition
			(
				{
					.weaponEntity = &mSword,
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

		Point velocity = direction->value * SPEED;
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
			swordTransform->position = direction->value * PLAYER_RADIUS * 3.141592f;
		}
	}

	// 총을 업데이트한다.
	{
		constexpr float PLAYER_RADIUS = 20.0f;
		setWeaponPosition
		(
			{
				.weaponEntity = &mGun,
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

		const Transform* gunTransform = mGun.GetComponent<Transform>();

		Bullet* bullet = mBullet.GetComponent<Bullet>();
		Transform* bulletTransform = mBullet.GetComponent<Transform>();
		Direction* bulletDirection = mBullet.GetComponent<Direction>();
		Animator* bulletAnim = mBullet.GetComponent<Animator>();
		Active* active = mBullet.GetComponent<Active>();
		WeaponState* bulletState = mBullet.GetComponent<WeaponState>();

		bulletState->fireCoolTimer += deltaTime;
		if (bulletState->fireCoolTimer >= FIRE_COOLTIME)
		{
			bulletAnim->frameIndex = 0;
			bulletAnim->elapsedTime = 0.0f;

			active->value = true;
			bulletTransform->position = gunTransform->position;

			const Point difference = getScreenMousePosition() - gunTransform->position;
			const float length = Math::GetVectorLength(difference);

			bulletDirection->value = difference / length;
			bulletState->fireCoolTimer = 0.0f;
		}

		Point velocity = bulletDirection->value * SPEED;
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
			constexpr float SPWAN_POSITION_TIME = 0.5f;
			constexpr float SPWAN_SCALE = 0.7f;
			monsterSpwan
			(
				{
					.entities = &mMonsters,
					.spwanPositionTime = SPWAN_POSITION_TIME,
					.rangeX = {.x = -150.0f, .xx = 150.0f },
					.rangeY = {.y = -150.0f, .yy = 150.0f },
					.spwanScale = SPWAN_SCALE,
					.maxHp = 1, 
					.deltaTime = deltaTime
				}
			);

			constexpr float ORIGNAL_SCALE = 4.0f;
			constexpr float SPWAN_WAITING_TIME = 1.0f;
			constexpr float ATTACK_DISTANCE = 90.0f;
			monsterState
			(
				{
					.entities = &mMonsters,
					.spwanPositionTime = SPWAN_POSITION_TIME,
					.spwanScale = SPWAN_SCALE,
					.originScale = ORIGNAL_SCALE,
					.spwanWaitingTime = SPWAN_WAITING_TIME,
					.attackDistance = ATTACK_DISTANCE,
					.deltaTime = deltaTime
				}
			);

			// 충돌했을 때 애니메이션 처리
			{
				for (Entity& entity : mMonsters)
				{
					Monster* monster = entity.GetComponent<Monster>();
					Hp* hp = entity.GetComponent<Hp>();

					if (monster->state != Monster::eState::Spwan
						and monster->state != Monster::eState::Dead)
					{
						if (Input::Get().GetKeyDown(SDL_SCANCODE_T))
						{
							if (monster->length < 90.0f)
							{
								hp->value -= 1;
							}
						}
					}
				}

				constexpr float DAMAGE_TIME = 0.3f;
				static int32_t prevHp[10]{};
				//for (uint32_t i = 0; i < mMonsters.size(); ++i)
				//{
				//	Entity entity = mMonsters[i];
				//	Monster* monster = entity.GetComponent<Monster>();
				//	DamageTimer* damage = entity.GetComponent<DamageTimer>();
				//	Hp* hp = entity.GetComponent<Hp>();
				//	prevHp[i] = hp->value;

				//	if (prevHp[i] != hp->value)
				//	{
				//		Color* color = entity.GetComponent<Color>();
				//		color->r = 200;
				//		color->g = 0;
				//		color->b = 0;

				//		damage->damageTimer += deltaTime;
				//		if (damage->damageTimer >= DAMAGE_TIME)
				//		{
				//			monster->state = Monster::eState::Run;
				//			Color* color = entity.GetComponent<Color>();
				//			color->r = 255;
				//			color->g = 255;
				//			color->b = 255;

				//			prevHp[i] = hp->value;
				//			damage->damageTimer = 0.0f;
				//		}
				//	}
				//}
			}

			// 파티클이 생성된다.
			constexpr float SPEED = 300.0f;
			constexpr float DEAD_TIME = 0.5f;
			monsterDeadParticle(&mMonsters, DEAD_TIME, SPEED, deltaTime);
		}

		float speed = getRandom(100.0f, 500.0f);
		monsterMove(&mMonsters, speed, deltaTime);

		monsterSetClip(&mMonsters, mMonsterClips);
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
			delete[] mTiles[y];
		}

		delete[] mTiles;
	}

	mDeadParticleTexture.Finalize();
}

void MainScene::initialize_Resource()
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

void MainScene::initialize_Entity()
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
		mTiles = new Entity * [height];

		for (uint32_t y = 0; y < height; ++y)
		{
			const float offsetY = float(height) * float(TILE_SIZE - 1);

			mTiles[y] = new Entity[width];

			for (uint32_t x = 0; x < width; ++x)
			{
				uint32_t tileIndex = 0;
				fscanf_s(file, "%d", &tileIndex);
				assert(mTileTextures.size() > tileIndex and "지원하지 않는 타일입니다.");

				Texture& tileTexture = mTileTextures[tileIndex];
				assert(tileTexture.GetWidth() == TILE_SIZE and tileTexture.GetHeight() == TILE_SIZE and "지원하지 않는 타일 사이즈입니다.");

				Entity& tile = mTiles[y][x];

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
		mPlayer.AddComponent(player);

		Direction direction{};
		mPlayer.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = SIZE, .height = SIZE };
		mPlayer.AddComponent(transform);

		mPlayerClips[uint32_t(Player::eState::Idle)].SetLoop(true);
		mPlayerClips[uint32_t(Player::eState::Run)].SetLoop(true);

		Animator animator{};
		animator.clipState = &mPlayerClips[uint32_t(Player::eState::Idle)];
		mPlayer.AddComponent(animator);

		Active active{};
		active.value = true;
		mPlayer.AddComponent(active);

		Color color{};
		mPlayer.AddComponent(color);

		GetEntityWorld()->AddEntity(&mPlayer);
	}

	// Sword
	{
		const float SIZE = 3.0f;
		constexpr Point CENTER = { .x = 0.0f,.y = 1.0f };

		Sword sword{};
		mSword.AddComponent(sword);

		WeaponState state{};
		mSword.AddComponent(state);

		Direction direction{};
		mSword.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = SIZE, .height = SIZE };
		transform.center = CENTER;
		mSword.AddComponent(transform);

		mSwordClip.SetLoop(true);

		Animator animator{};
		animator.clipState = &mSwordClip;
		mSword.AddComponent(animator);

		Active active{};
		active.value = true;
		mSword.AddComponent(active);

		Color color{};
		mSword.AddComponent(color);

		GetEntityWorld()->AddEntity(&mSword);
	}

	// Gun
	{
		constexpr float SIZE = 3.0f;
		constexpr Point CENTER = { .x = 0.0f,.y = 0.5f };

		Gun gun{};
		mGun.AddComponent(gun);

		Direction direction{};
		mGun.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = 3.0f, .height = 3.0f };
		transform.center = CENTER;
		mGun.AddComponent(transform);

		Image image;
		image.texture = &mGunTexture;
		mGun.AddComponent(image);

		Active active{};
		active.value = true;
		mGun.AddComponent(active);

		Color color{};
		mGun.AddComponent(color);

		GetEntityWorld()->AddEntity(&mGun);
	}

	// Bullet
	{
		const float SIZE = 2.0f;

		Bullet bullet{};
		mBullet.AddComponent(bullet);

		WeaponState state{};
		mBullet.AddComponent(state);

		Direction direction{};
		mBullet.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = SIZE, .height = SIZE };
		mBullet.AddComponent(transform);

		mBulletClip.SetLoop(true);

		Animator animator{};
		animator.clipState = &mBulletClip;
		mBullet.AddComponent(animator);

		Active active{};
		mBullet.AddComponent(active);

		Color color{};
		mBullet.AddComponent(color);

		GetEntityWorld()->AddEntity(&mBullet);
	}

	// Monster
	{
		constexpr uint32_t MONSTER_COUNT = 3;

		mMonsterClips[uint32_t(Monster::eState::Spwan)].SetLoop(true);
		mMonsterClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mMonsterClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		mMonsters.reserve(MONSTER_COUNT);
		for (uint32_t i = 0; i < MONSTER_COUNT; ++i)
		{
			Entity& entity = mMonsters.emplace_back();

			Monster monster{};
			monster.state = Monster::eState::Dead;
			entity.AddComponent(monster);

			Direction direction{};
			entity.AddComponent(direction);

			SpwanTimer spwanTimer{};
			entity.AddComponent(spwanTimer);

			DamageTimer damageTimer{};
			entity.AddComponent(damageTimer);

			Hp hp{};
			entity.AddComponent(hp);

			Transform transform{};
			entity.AddComponent(transform);

			Animator animator{};
			animator.clipState = &mMonsterClips[uint32_t(Monster::eState::Dead)];
			entity.AddComponent(animator);

			Active active{};
			entity.AddComponent(active);

			Color color{};
			entity.AddComponent(color);

			GetEntityWorld()->AddEntity(&entity);
		}
	}

	// Dead Particle
	{
		constexpr float SIZE = 0.3f;

		for (Entity& entity : mDeadParticle)
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

void MainScene::input()
{
	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
	{
		mIsUpdate = false;
	}
}

void MainScene::playerState()
{
	Player* player = mPlayer.GetComponent<Player>();
	player->state = (player->length != 0.0f) ? Player::eState::Run : Player::eState::Idle;
}

void MainScene::playerMove(const float deltaTime)
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

	Player* player = mPlayer.GetComponent<Player>();
	player->length = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	Direction* direction = mPlayer.GetComponent<Direction>();

	if (player->length > 0.0f)
	{
		direction->value = { .x = velocity.x / player->length, .y = velocity.y / player->length };

		velocity = direction->value * MAX_SPEED;
	}

	Transform* transform = mPlayer.GetComponent<Transform>();
	transform->position = transform->position + velocity * deltaTime;
	transform->flip = (direction->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}

void MainScene::playerSetClip()
{
	Player* player = mPlayer.GetComponent<Player>();
	Animator* animator = mPlayer.GetComponent<Animator>();

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

void MainScene::monsterSpwan(const MonsterSpwanDesc& desc)
{
	std::vector<Entity>* entities = desc.entities;
	const float spwanPositionTime = desc.spwanPositionTime;
	const RangeX rangeX = desc.rangeX;
	const RangeY rangeY = desc.rangeY;
	const float spwanScale = desc.spwanScale;
	const float deltaTime = desc.deltaTime;
	const float maxHp = desc.maxHp;
	static uint32_t spwanIndex{};

	for (uint32_t i = 0; i < entities->size(); ++i) 
	{
		Entity& entity = (*entities)[i];

		Monster* monster = entity.GetComponent<Monster>();
		Transform* monsterTransform = entity.GetComponent<Transform>();
		Active* active = entity.GetComponent<Active>();
		SpwanTimer* spwan = entity.GetComponent<SpwanTimer>();
		Hp* hp = entity.GetComponent<Hp>();

		if (spwanIndex == i
			and not active->value
			and not spwan->isSpwan)
		{
			hp->max = maxHp;

			spwan->spwanPositionTimer += deltaTime;
			if (spwan->spwanPositionTimer >= spwanPositionTime)
			{
				monster->state = Monster::eState::Spwan;
				spwan->isSpwan = true;
				monsterTransform->position = 
				{ 
					.x = getRandom(rangeX.x, rangeX.xx), 
					.y = getRandom(rangeY.y, rangeY.yy) 
				};
				monsterTransform->scale = { .width = spwanScale, .height = spwanScale };
				active->value = true;

				spwanIndex++;

				spwan->spwanWaitingTimer = 0.0f;
				spwan->spwanPositionTimer = 0.0f;
			}
		}

		if (spwanIndex == entities->size())
		{
			spwanIndex = 0;
		}
	}
}

void MainScene::monsterState(const MonsterStateDesc& desc)
{
	std::vector<Entity>* entities = desc.entities;
	const float originScale = desc.originScale;
	const float spwanWaitingTime = desc.spwanWaitingTime;
	const float attackDistance = desc.attackDistance;
	const float deltaTime = desc.deltaTime;

	for (Entity& entity : *entities)
	{
		Monster* monster = entity.GetComponent<Monster>();
		Transform* monsterTransform = entity.GetComponent<Transform>();
		Active* active = entity.GetComponent<Active>();
		SpwanTimer* spwan = entity.GetComponent<SpwanTimer>();

		Animator* anim = entity.GetComponent<Animator>();
		const Clip& attackClip = mMonsterClips[uint32_t(Monster::eState::Attack)];

		if (monster->state == Monster::eState::Spwan)
		{
			spwan->spwanWaitingTimer += deltaTime;

			float waitingTime = spwanWaitingTime / 2.0f;
			if (spwan->spwanWaitingTimer >= waitingTime)
			{
				spwan->spwanBlinkTimer += deltaTime;
				if (spwan->spwanBlinkTimer >= 0.06f)
				{
					active->value = !active->value;
					spwan->spwanBlinkTimer = 0.0f;
				}
			}
			if (spwan->spwanWaitingTimer >= spwanWaitingTime)
			{
				monsterTransform->scale = { .width = originScale, .height = originScale };

				monster->state = Monster::eState::Run;
				active->value = true;
				spwan->spwanBlinkTimer = 0.0f;
				spwan->spwanWaitingTimer = 0.0f;
			}
		}
		else if (monster->state == Monster::eState::Attack)
		{
			if (anim->clipState == &attackClip
				and anim->frameIndex >= attackClip.GetLastFrameIndex() - 1)
			{
				if (monster->length > attackDistance)
				{
					monster->state = Monster::eState::Run;
				}
			}
		}
		else if (monster->state == Monster::eState::Run)
		{
			if (monster->length <= attackDistance)
			{
				monster->state = Monster::eState::Attack;
			}
		}
		else
		{

		}
	}
}

void MainScene::monsterDeadParticle(std::vector<Entity>* entities, const float deadTime, const float speed, const float deltaTime)
{
	for (Entity& entity : *entities)
	{
		Hp* hp = entity.GetComponent<Hp>();
		Monster* monster = entity.GetComponent<Monster>();
		Transform* monsterTransform = entity.GetComponent<Transform>();
		SpwanTimer* spwan = entity.GetComponent<SpwanTimer>();

		if (hp->value <= 0)
		{
			if (monster->state != Monster::eState::Dead
				and monster->state != Monster::eState::Spwan)
			{
				for (Entity& entity : mDeadParticle)
				{
					Particle* particle = entity.GetComponent<Particle>();
					Direction* direction = entity.GetComponent<Direction>();
					Point& dir = direction->value;

					dir = getScreenMousePosition() - monsterTransform->position;
					dir = Math::NormalizeVector(dir);
					dir = Math::RotatePoint(dir, getRandom(-60.0f, 60.0f));

					Transform* transform = entity.GetComponent<Transform>();
					transform->position = monsterTransform->position;
				}
			}

			monster->state = Monster::eState::Dead;
			Active* monsterActive = entity.GetComponent<Active>();
			monsterActive->value = false;

			for (Entity& entity : mDeadParticle)
			{
				if (not monsterActive->value
					and spwan->isSpwan)
				{
					Active* particleActive = entity.GetComponent<Active>();
					particleActive->value = true;

					Transform* transform = entity.GetComponent<Transform>();
					Particle* particle = entity.GetComponent<Particle>();
					Direction* direction = entity.GetComponent<Direction>();
					transform->position = transform->position + direction->value * speed * deltaTime;
				}
			}

			DamageTimer* damage = entity.GetComponent<DamageTimer>();
			SpwanTimer* spwan = entity.GetComponent<SpwanTimer>();

			damage->deadTimer += deltaTime;
			if (damage->deadTimer >= deadTime)
			{
				for (Entity& entity : mDeadParticle)
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
}

void MainScene::monsterMove(std::vector<Entity>* entities, const float maxSpeed,  const float deltaTime)
{
	Point velocity = {};

	for (Entity& entity : *entities)
	{
		Monster* monster = entity.GetComponent<Monster>();
		Transform* monsterTransform = entity.GetComponent<Transform>();
		const Transform* playerTransform = mPlayer.GetComponent<Transform>();

		const Point monsterPosition = monsterTransform->position;
		const Point playerPosition = playerTransform->position;

		const Point difference = playerPosition - monsterPosition;
		monster->length = Math::GetVectorLength(difference);

		Direction* direction = entity.GetComponent<Direction>();
		if (monster->state == Monster::eState::Run)
		{
			if (monster->length > 0.0f)
			{
				direction->value = difference / monster->length;
				velocity = direction->value * maxSpeed;
			}

			monsterTransform->position = monsterTransform->position + velocity * deltaTime;
			monsterTransform->flip = (direction->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		}
	}
}

void MainScene::monsterSetClip(std::vector<Entity>* entities, std::array<Clip, uint32_t(Monster::eState::Count)>& clips)
{
	for (Entity& entity : *entities)
	{
		Animator* animator = entity.GetComponent<Animator>();
		Monster* monster = entity.GetComponent<Monster>();

		switch (monster->state)
		{
		case Monster::eState::Spwan:
			animator->SetClip(&clips[uint32_t(Monster::eState::Spwan)]);
			break;

		case Monster::eState::Idle:
			animator->SetClip(&clips[uint32_t(Monster::eState::Idle)]);
			break;

		case Monster::eState::Run:
			animator->SetClip(&clips[uint32_t(Monster::eState::Run)]);
			break;

		case Monster::eState::Attack:
			animator->SetClip(&clips[uint32_t(Monster::eState::Attack)]);
			break;

		case Monster::eState::Dead:
			animator->SetClip(&clips[uint32_t(Monster::eState::Idle)]);
			break;

		default:
			assert(false and "지원하지 않는 애니메이션입니다.");
			break;
		}
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
	const float playerRadius = desc.playerRadius;
	const float dgreeOffset = desc.dgreeOffset;
	const SDL_RendererFlip flipX = desc.flipX;
	const SDL_RendererFlip flipY = desc.flipY;

	Transform* transform = weaponEntity->GetComponent<Transform>();
	Direction* direction = weaponEntity->GetComponent<Direction>();

	const Transform* playerTransform = mPlayer.GetComponent<Transform>();
	const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;
	float degree = std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);
	degree = degree + dgreeOffset;
	transform->angle = -degree;

	const float length = Math::GetVectorLength(mouseToPlayer);
	direction->value = mouseToPlayer / length;

	transform->position = playerTransform->position + direction->value * playerRadius * 3.141592f;
	transform->flip = (direction->value.x > 0.0f) ? flipX : flipY;
}

float MainScene::getRandom(const float min, const float max)
{
	const float result = float(rand()) / RAND_MAX * (max - min) + min;
	return result;
}
