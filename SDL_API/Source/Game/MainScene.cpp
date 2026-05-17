#include "pch.h"
#include "Core/Entity/EntityWorld.h"

#include "Core/Collision.h"
#include "Core/Constant.h"
#include "Core/Helper.h"
#include "Core/Input.h"

#include "MainScene.h"

constexpr float PRIMARY_SIZE = 3.0f;

// Core
struct CollisionDetector final : public Component
{
	static constexpr uint32_t _ID = 0;
	CollisionDetector(const uint32_t layer) : Component(&_ID), Layer(layer) {}

	const uint32_t Layer = 0;
	std::bitset<64> CollisionLayerMask{};
};

struct BoxCollider final : public Component
{
	static constexpr uint32_t _ID = 0;
	BoxCollider() : Component(&_ID) {}

	Point offset{};
	Scale size{};
};

struct CircleCollider final : public Component
{
	static constexpr uint32_t _ID = 0;
	CircleCollider() : Component(&_ID) {}

	Point offset{};
	float radius{};
};

struct LineCollider final : public Component
{
	static constexpr uint32_t _ID = 0;
	LineCollider() : Component(&_ID) {}

	Point offset{};
	Scale scale{};
};

void MainScene::Initialize()
{
	initialize_Resource();

	initialize_Entity();
}

bool MainScene::Update(const float deltaTime)
{
	// Core
	{
		for (const Entity* entity0 : GetEntityWorld()->GetAllEntites())
		{
			if (not entity0->HasComponent<Transform>()
				or not entity0->HasComponent<CollisionDetector>())
			{
				continue;
			}

			CollisionDetector* collisionDetector0 = entity0->GetComponent<CollisionDetector>();
			if (collisionDetector0->CollisionLayerMask.none())
			{
				continue;
			}

			for (const Entity* entity1 : GetEntityWorld()->GetAllEntites())
			{
				if (not entity1->HasComponent<Transform>()
					or not entity1->HasComponent<CollisionDetector>())
				{
					continue;
				}

				if (entity0 == entity1)
				{
					continue;
				}

				CollisionDetector* collisionDetector1 = entity1->GetComponent<CollisionDetector>();
				if (not collisionDetector0->CollisionLayerMask[collisionDetector1->Layer])
				{
					continue;
				}

				{
					checkCollisionBoxBox(*entity0, *entity1)
						/*or checkCollisionBoxCircle(*entity0, *entity1)
						or checkCollisionBoxLine(*entity0, *entity1)
						or checkCollisionCircleCircle(*entity0, *entity1)
						or checkCollisionCircleLine(*entity0, *entity1)*/;
				}
			}
		}
	}

	// 플레이어를 업데이트한다.
	{
		input();

		playerState(deltaTime);

		playerMove(deltaTime);
	}

	// 카메라를 업데이트한다.
	{
		Transform* transform = mMainCamera.GetComponent<Transform>();
		Transform* target = mPlayer.GetComponent<Transform>();
		transform->position = target->position;

		constexpr float OFFSET = 31.0f;
		const Scale halfScreen =
		{
			.width = float(Constant::Get().GetWidth()) * 0.5f - OFFSET,
			.height = float(Constant::Get().GetHeight()) * 0.5f - OFFSET,
		};

		clampToTile(transform, { .x = halfScreen.width, .xx = halfScreen.width }, { .y = halfScreen.height, .yy = halfScreen.height });
	}

	// 칼을 업데이트한다.
	{
		const Transform* playerTransform = mPlayer.GetComponent<Transform>();

		const Point offset =
		{
			.x = (playerTransform->flip == SDL_FLIP_NONE) ? -60.0f : 60.0f,
			.y = 80.0f
		};
		const Point targetPosition = offset + playerTransform->position;

		Transform* swordTransform = mSword.GetComponent<Transform>();
		swordTransform->position = Math::LerpVector(swordTransform->position, targetPosition, 0.16f);
	}

	// 칼의 이펙트를 업데이트한다.
	{
		Transform* effectTransform = mSwordSkill.GetComponent<Transform>();
		Direction* effectDirection = mSwordSkill.GetComponent<Direction>();
		
		const Transform* playerTransform = mPlayer.GetComponent<Transform>();

		Active* active = mSwordSkill.GetComponent<Active>();
		Effect* effect = mSwordSkill.GetComponent<Effect>();
		if (Input::Get().GetMouseButtonDown(SDL_BUTTON_LEFT)
			and not active->isValue
			and not effect->isDisabled)
		{
			active->isValue = true;

			effectTransform->position = playerTransform->position;
			const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;
			effectDirection->value = Math::NormalizeVector(mouseToPlayer);

			float degree = std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);
			degree -= 90.0f;
			effectTransform->angle = -degree;

			effectTransform->flip = (effectDirection->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		}

		constexpr float SPEED = 900.0f;
		Point velocity = effectDirection->value * SPEED;
		effectTransform->position = effectTransform->position + velocity * deltaTime;

		constexpr float LENGTH = 15.0f;
		constexpr float RANGE = LENGTH * LENGTH;
		constexpr float SWING_COOLTIME = 0.7f;
		if (active->isValue)
		{
			const Point toEffect = effectTransform->position - playerTransform->position;
			float length = Math::GetVectorLength(toEffect);

			if (length >= RANGE)
			{
				Animator* effectAnim = mSwordSkill.GetComponent<Animator>();
				effectAnim->frameIndex = 0;
				effectAnim->elapsedTime = 0.0f;

				active->isValue = false;
				effect->isDisabled = true;
			}
		}
		else
		{
			effect->coolTimer += deltaTime;
			if (effect->coolTimer >= SWING_COOLTIME)
			{
				effect->isDisabled = false;
				effect->coolTimer = 0.0f;
			}
		}
	}

	// 총을 업데이트한다.
	{
		setWeaponPosition
		(
			{
				.weaponEntity = &mGun,
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

			active->isValue = true;
			bulletTransform->position = gunTransform->position;

			const Point difference = getScreenMousePosition() - gunTransform->position;
			const float length = Math::GetVectorLength(difference);

			bulletDirection->value = difference / length;
			bulletState->fireCoolTimer = 0.0f;
		}

		Point velocity = bulletDirection->value * SPEED;
		bulletTransform->position = bulletTransform->position + velocity * deltaTime;

		const Point toBullet = bulletTransform->position - gunTransform->position;
		const float distSq = Math::GetVectorLength(toBullet);
		const float rangeSq = LENGTH * LENGTH;
		
		if (distSq >= rangeSq)
		{
			active->isValue = false;
		}
	}

	// 몬스터를 업데이트한다.
	{
		// Big White
		{
			// State
			{
				constexpr float SPWAN_POSITION_TIME = 0.5f;
				constexpr float SPWAN_SCALE = 0.7f;
				monsterSpwan
				(
					MonsterSpwanDesc
					{
						.entities = &mBigWhiteSkels,
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
						.entities = &mBigWhiteSkels,
						.attackClip = mBigWhiteSkelClips[uint32_t(Monster::eState::Attack)],
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
					//	constexpr float DAMAGE_TIME = 0.3f;
					//	static int32_t prevHp[10]{};
					//	for (uint32_t i = 0; i < mMonsters.size(); ++i)
					//	{
					//		Entity entity = mMonsters[i];
					//		Monster* monster = entity.GetComponent<Monster>();
					//		DamageTimer* damage = entity.GetComponent<DamageTimer>();
					//		Hp* hp = entity.GetComponent<Hp>();
					//		prevHp[i] = hp->value;

					//		if (prevHp[i] != hp->value)
					//		{
					//			Color* color = entity.GetComponent<Color>();
					//			color->r = 200;
					//			color->g = 0;
					//			color->b = 0;

					//			damage->damageTimer += deltaTime;
					//			if (damage->damageTimer >= DAMAGE_TIME)
					//			{
					//				monster->state = Monster::eState::Run;
					//				Color* color = entity.GetComponent<Color>();
					//				color->r = 255;
					//				color->g = 255;
					//				color->b = 255;

					//				prevHp[i] = hp->value;
					//				damage->damageTimer = 0.0f;
					//			}
					//		}
					//	}
				}

				// 파티클이 생성된다.
				constexpr float SPEED = 300.0f;
				constexpr float DEAD_TIME = 0.5f;
				monsterDeadParticle(&mBigWhiteSkels, DEAD_TIME, SPEED, deltaTime);
			}

			float speed = getRandom(100.0f, 500.0f);
			monsterMove(&mBigWhiteSkels, speed, deltaTime);
		}

		// Archer
		{
			// State
			{
				constexpr float SPWAN_POSITION_TIME = 0.7f;
				constexpr float SPWAN_SCALE = 0.7f;
				monsterSpwan
				(
					MonsterSpwanDesc
					{
						.entities = &mArchers,
						.spwanPositionTime = SPWAN_POSITION_TIME,
						.rangeX = {.x = -300.0f, .xx = 300.0f },
						.rangeY = {.y = -300.0f, .yy = 300.0f },
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
						.entities = &mArchers,
						.attackClip = mArcherClips[uint32_t(Monster::eState::Attack)],
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
					//	constexpr float DAMAGE_TIME = 0.3f;
					//	static int32_t prevHp[10]{};
					//	for (uint32_t i = 0; i < mMonsters.size(); ++i)
					//	{
					//		Entity entity = mMonsters[i];
					//		Monster* monster = entity.GetComponent<Monster>();
					//		DamageTimer* damage = entity.GetComponent<DamageTimer>();
					//		Hp* hp = entity.GetComponent<Hp>();
					//		prevHp[i] = hp->value;

					//		if (prevHp[i] != hp->value)
					//		{
					//			Color* color = entity.GetComponent<Color>();
					//			color->r = 200;
					//			color->g = 0;
					//			color->b = 0;

					//			damage->damageTimer += deltaTime;
					//			if (damage->damageTimer >= DAMAGE_TIME)
					//			{
					//				monster->state = Monster::eState::Run;
					//				Color* color = entity.GetComponent<Color>();
					//				color->r = 255;
					//				color->g = 255;
					//				color->b = 255;

					//				prevHp[i] = hp->value;
					//				damage->damageTimer = 0.0f;
					//			}
					//		}
					//	}
				}

				// 파티클이 생성된다.
				constexpr float SPEED = 300.0f;
				constexpr float DEAD_TIME = 0.5f;
				monsterDeadParticle(&mBigWhiteSkels, DEAD_TIME, SPEED, deltaTime);
			}

			float speed = getRandom(50.0f, 400.0f);
			monsterMove(&mArchers, speed, deltaTime);
		}
	}

	Hp* playerHp = mPlayer.GetComponent<Hp>();
	Label* playerLabel = mPlayer.GetComponent<Label>();
	Knockback* knockback = mPlayer.GetComponent<Knockback>();

	for (const auto& monster : mBigWhiteSkels)
	{	
		if (isCollisionEnter(mPlayer, monster))
		{
			if (monster.GetComponent<Monster>()->state == Monster::eState::Run)
			{
				playerHp->value -= 1;
				std::string name = "Hp: " + std::to_string(playerHp->value);
				playerLabel->SetText(GetHelper(), name);
			}
		}
		else if (isCollisionStay(mPlayer, monster))
		{
			if (monster.GetComponent<Monster>()->state == Monster::eState::Attack)
			{
				knockback->isValue = true;
			}
		}		
	}

	{
		mPreviousCollidedEntityPairs = mCollidedEntityPairs;
		mCollidedEntityPairs.clear();
	}

	playerSetClip();
	monsterSetClip(&mBigWhiteSkels, mBigWhiteSkelClips);
	monsterSetClip(&mArchers, mArcherClips);

	return mIsUpdate;
}

void MainScene::Finalize()
{
	mUIFont.Finalize();

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

		mPlayerDeadTexture.Finalize();
	}

	// Monster
	{
		mArrowTexture.Finalize();

		mSpwanTexture.Finalize();
		mMonsterIdleTexture.Finalize();

		for (Texture& texture : mMonsterRunTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mMonsterAttackTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mArcherIdleTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mArcherRunTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mArcherAttackTextures)
		{
			texture.Finalize();
		}
	}

	for (Texture& texture : mSwordTextures)
	{
		texture.Finalize();
	}

	for (Texture& texture : mSwordSkillTextures)
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
		for (uint32_t y = 0; y < mTileMaxCount; ++y)
		{
			delete[] mTiles[y];
		}

		delete[] mTiles;
	}

	mDeadParticleTexture.Finalize();
}

void MainScene::initialize_Resource()
{
	mUIFont.Initilize("Resource/DroidSans.TTF", 50);
	mHpFont.Initilize("Resource/DroidSans.TTF", 20);

	// Tile
	mTileTextures[0].Initialize(GetHelper(), "Resource/Tile/0.png");
	mTileTextures[1].Initialize(GetHelper(), "Resource/Tile/1.png");

	mDeadParticleTexture.Initialize(GetHelper(), "Resource/RedRectangle.png");

	// Player
	{
		constexpr Point IDLE_OFFSETS[]
		{
			{ 0.0f, -0.50f },
			{ 0.0f, -0.50f },
			{ 0.0f, -0.50f },
			{ 0.0f, -0.50f },
			{ 0.0f, -0.50f },
		};
		assert(std::size(IDLE_OFFSETS) == mPlayerIdleTextures.size() and "offset 개수와 texture 개수가 일치하지 않습니다.");

		uint32_t index{};
		for (Texture& texture : mPlayerIdleTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Char/Alice/Idle/" + std::to_string(index) + ".png");

			Clip::Frame frame =
			{
				.texture = &texture, 
				.durationTime = 0.12f, 
				.center = IDLE_OFFSETS[index++]
			};
			mPlayerClips[uint32_t(Player::eState::Idle)].AddClip(frame);
		}

		constexpr Point RUN_OFFSETS[]
		{
			{ -0.09f, -0.50f },
			{ -0.06f, -0.50f },
			{ -0.06f, -0.50f },
			{ -0.09f, -0.50f },
			{ -0.06f, -0.50f },
			{ -0.06f, -0.50f }
		};
		assert(std::size(RUN_OFFSETS) == mPlayerRunTextures.size() and "offset 개수와 texture 개수가 일치하지 않습니다.");

		index = 0;
		for (Texture& texture : mPlayerRunTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Char/Alice/Run/" + std::to_string(index) + ".png");

			Clip::Frame frame = 
			{
				.texture = &texture, 
				.durationTime = 0.12f, 
				.center = RUN_OFFSETS[index++]
			};

			mPlayerClips[uint32_t(Player::eState::Run)].AddClip(frame);
		}

		mPlayerDeadTexture.Initialize(GetHelper(), "Resource/Char/Alice/Dead/0.png");
		Clip::Frame frame =
		{
			.texture = &mPlayerDeadTexture,
		};
		mPlayerClips[uint32_t(Player::eState::Dead)].AddClip(frame);
	}

	// Sword
	{
		uint32_t cnt{};
		for (Texture& texture : mSwordTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Sword/" + std::to_string(cnt++) + ".png");

			Clip::Frame frame = 
			{
				.texture = &texture, 
				.durationTime = 0.12f
			};

			mSwordClip.AddClip(frame);
		}
	}

	// Sword Skill
	{
		uint32_t index{};
		for (Texture& texture : mSwordSkillTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Sword/Skill/" + std::to_string(index++) + ".png");

			Clip::Frame frame = 
			{
				.texture = &texture, 
				.durationTime = 0.12f
			};

			mSwordSkillClip.AddClip(frame);
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

			Clip::Frame frame = 
			{
				.texture = &texture, 
				.durationTime = 0.02f
			};

			mBulletClip.AddClip(frame);
		}
	}
	
	mSpwanTexture.Initialize(GetHelper(), "Resource/Monster/Effect/Die/Die01.png");
	uint32_t index{};

	// Big White
	{	
		// Spwan
		{
			Clip::Frame frame =
			{
				.texture = &mSpwanTexture,
				.durationTime = 0.12f
			};

			mBigWhiteSkelClips[uint32_t(Monster::eState::Spwan)].AddClip(frame);
		}

		// Idle
		{
			mMonsterIdleTexture.Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Idle/0.png");

			Clip::Frame frame = 
			{
				.texture = &mMonsterIdleTexture, 
				.durationTime = 0.12f
			};

			mBigWhiteSkelClips[uint32_t(Monster::eState::Idle)].AddClip(frame);
		}

		// Run
		for (Texture& texture : mMonsterRunTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Run/" + std::to_string(index++) + ".png");

			Clip::Frame frame = 
			{
				.texture = &texture, 
				.durationTime = 0.12f
			};

			mBigWhiteSkelClips[uint32_t(Monster::eState::Run)].AddClip(frame);
		}

		index = 0;
		for (Texture& texture : mMonsterAttackTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Monster/AbyssKnight/Attack/" + std::to_string(index++) + ".png");

			Clip::Frame frame = 
			{
				.texture = &texture, 
				.durationTime = 0.12f
			};

			mBigWhiteSkelClips[uint32_t(Monster::eState::Attack)].AddClip(frame);
		}
		index = 0;
	}

	// Archer
	{
		// Spwan
		{
			Clip::Frame frame =
			{
				.texture = &mSpwanTexture,
				.durationTime = 0.12f
			};
			mArcherClips[uint32_t(Monster::eState::Spwan)].AddClip(frame);
		}

		// Idle
		{
			for (Texture& texture : mArcherIdleTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Archer/Idle/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};
				mArcherClips[uint32_t(Monster::eState::Idle)].AddClip(frame);
			}
			index = 0;
		}

		// Run
		{
			for (Texture& texture : mArcherRunTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Archer/Run/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};
				mArcherClips[uint32_t(Monster::eState::Run)].AddClip(frame);
			}
			index = 0;
		}

		// Attack
		{
			for (Texture& texture : mArcherAttackTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Archer/Attack/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};
				mArcherClips[uint32_t(Monster::eState::Attack)].AddClip(frame);
			}
			index = 0;
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
		mTilePositionOffset = TILE_SIZE * PRIMARY_SIZE;

		FILE* file = nullptr;
		fopen_s(&file, "Resource/Tile/Tile.txt", "r");
		assert(file != nullptr);

		uint32_t width{};
		uint32_t height{};
		fscanf_s(file, "%d %d", &width, &height);
		mTileMaxCount = height;


		Point centerOffset = { .x = -float(width * 0.5f), .y = -float(height * 0.5f), };

		mTiles = new Entity * [height];
		for (uint32_t y = 0; y < height; ++y)
		{
			const float offsetY = float(height + centerOffset.y) * float(TILE_SIZE - 1);

			mTiles[y] = new Entity[width];

			for (uint32_t x = 0; x < width; ++x)
			{
				uint32_t tileIndex = 0;
				fscanf_s(file, "%d", &tileIndex);
				assert(mTileTextures.size() > tileIndex and "지원하지 않는 타일입니다.");

				Texture& tileTexture = mTileTextures[tileIndex];
				assert(tileTexture.GetWidth() == TILE_SIZE and tileTexture.GetHeight() == TILE_SIZE and "지원하지 않는 타일 사이즈입니다.");

				Entity& entity = mTiles[y][x];

				Transform transform{};
				transform.position = { .x = (x + centerOffset.x) * TILE_SIZE * PRIMARY_SIZE, .y = (offsetY - y * TILE_SIZE) * PRIMARY_SIZE };
				transform.scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
				entity.AddComponent(transform);

				Image image{};
				image.texture = &tileTexture;
				entity.AddComponent(image);

				Active active{};
				active.isValue = true;
				entity.AddComponent(active);

				Color color{};
				entity.AddComponent(color);

				GetEntityWorld()->AddEntity(&entity);
			}
		}

		fclose(file);
	}

	// UI Label
	{
		UI ui{};
		mLabelEntity.AddComponent(ui);

		Label label;
		label.font = &mUIFont;
		label.active = true;
		label.SetText(GetHelper(), "UI Label");
		mLabelEntity.AddComponent(label);

		Transform transform{};
		mLabelEntity.AddComponent(transform);

		Color color{};
		mLabelEntity.AddComponent(color);

		GetEntityWorld()->AddEntity(&mLabelEntity);
	}

	// Player
	{
		Player player{};
		player.state = Player::eState::Idle;
		mPlayer.AddComponent(player);

		Direction direction{};
		direction.value = { .x = 1.0f, .y = 1.0f };
		mPlayer.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
		mPlayer.AddComponent(transform);

		mPlayerClips[uint32_t(Player::eState::Idle)].SetLoop(true);
		mPlayerClips[uint32_t(Player::eState::Run)].SetLoop(true);

		Image image{};
		mPlayer.AddComponent(image);

		Animator animator{};
		animator.clipState = &mPlayerClips[uint32_t(Player::eState::Idle)];
		mPlayer.AddComponent(animator);

		Hp hp{};
		hp.max = 1;
		hp.value = hp.max;
		mPlayer.AddComponent(hp);

		Label label;
		label.font = &mHpFont;
		label.active = true;
		std::string name = "Hp: " + std::to_string(hp.value);
		label.SetText(GetHelper(), name);
		mPlayer.AddComponent(label);

		Knockback knockback{};
		mPlayer.AddComponent(knockback);

		Active active{};
		active.isValue = true;
		mPlayer.AddComponent(active);

		Color color{};
		mPlayer.AddComponent(color);

		CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
		mPlayer.AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.size = { .width = float(mPlayerRunTextures[3].GetWidth()), .height = float(mPlayerRunTextures[3].GetHeight())};
		mPlayer.AddComponent(boxCollider);

		GetEntityWorld()->AddEntity(&mPlayer);
	}

	// Sword
	{
		const float SIZE = 3.0f;

		Sword sword{};
		mSword.AddComponent(sword);

		WeaponState state{};
		mSword.AddComponent(state);

		Direction direction{};
		mSword.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = SIZE, .height = SIZE };
		transform.center = { .x = 0.0f,.y = -0.33f };
		mSword.AddComponent(transform);

		mSwordClip.SetLoop(true);

		Image image{};
		mSword.AddComponent(image);

		Animator animator{};
		animator.clipState = &mSwordClip;
		mSword.AddComponent(animator);

		Active active{};
		active.isValue = true;
		mSword.AddComponent(active);

		Color color{};
		mSword.AddComponent(color);

		GetEntityWorld()->AddEntity(&mSword);
	}

	// Sword Skill
	{
		const float SIZE = 2.5f;

		Effect effect{};
		mSwordSkill.AddComponent(effect);

		WeaponState state{};
		mSwordSkill.AddComponent(state);

		Direction direction{};
		mSwordSkill.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = SIZE, .height = SIZE };
		mSwordSkill.AddComponent(transform);

		mSwordSkillClip.SetLoop(true);

		Image image{};
		mSwordSkill.AddComponent(image);

		Animator animator{};
		animator.clipState = &mSwordSkillClip;
		mSwordSkill.AddComponent(animator);

		Active active{};
		mSwordSkill.AddComponent(active);

		Color color{};
		mSwordSkill.AddComponent(color);

		GetEntityWorld()->AddEntity(&mSwordSkill);
	}

	// Gun
	{
		constexpr Point CENTER = { .x = -0.5f,.y = 0.0f };

		Gun gun{};
		mGun.AddComponent(gun);

		Direction direction{};
		mGun.AddComponent(direction);

		Transform transform{};
		transform.scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
		transform.center = CENTER;
		mGun.AddComponent(transform);

		Image image;
		image.texture = &mGunTexture;
		mGun.AddComponent(image);

		Active active{};
		active.isValue = true;
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

		Image image{};
		mBullet.AddComponent(image);

		Animator animator{};
		animator.clipState = &mBulletClip;
		mBullet.AddComponent(animator);

		Active active{};
		mBullet.AddComponent(active);

		Color color{};
		mBullet.AddComponent(color);

		GetEntityWorld()->AddEntity(&mBullet);
	}

	// Big White
	{
		mBigWhiteSkelClips[uint32_t(Monster::eState::Spwan)].SetLoop(true);
		mBigWhiteSkelClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mBigWhiteSkelClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		constexpr uint32_t KNIGHT_MONSTER_COUNT = 1;
		mBigWhiteSkels.reserve(KNIGHT_MONSTER_COUNT);
		for (uint32_t i = 0; i < KNIGHT_MONSTER_COUNT; ++i)
		{
			Entity& entity = mBigWhiteSkels.emplace_back();

			Monster monster{};
			monster.state = Monster::eState::None;
			entity.AddComponent(monster);

			Transform transform{};
			entity.AddComponent(transform);

			Image image{};
			entity.AddComponent(image);

			Animator animator{};
			entity.AddComponent(animator);

			Direction direction{};
			entity.AddComponent(direction);

			SpwanTimer spwanTimer{};
			entity.AddComponent(spwanTimer);

			DamageTimer damageTimer{};
			entity.AddComponent(damageTimer);

			Hp hp{};
			entity.AddComponent(hp);

			Active active{};
			entity.AddComponent(active);

			Color color{};
			entity.AddComponent(color);

			CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
			collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
			entity.AddComponent(collider);

			BoxCollider boxCollider{};
			boxCollider.size = { .width = float(mMonsterAttackTextures[3].GetWidth()), .height = float(mMonsterAttackTextures[3].GetHeight()) };
			entity.AddComponent(boxCollider);

			GetEntityWorld()->AddEntity(&entity);
		}

		// Archer
		{
			mArcherClips[uint32_t(Monster::eState::Spwan)].SetLoop(true);
			mArcherClips[uint32_t(Monster::eState::Idle)].SetLoop(true);
			mArcherClips[uint32_t(Monster::eState::Run)].SetLoop(true);
			mArcherClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

			constexpr uint32_t ARCHER_COUNT = 1;
			mArchers.reserve(ARCHER_COUNT);
			for (uint32_t i = 0; i < ARCHER_COUNT; ++i)
			{
				Entity& entity = mArchers.emplace_back();

				Monster monster{};
				monster.state = Monster::eState::None;
				entity.AddComponent(monster);

				Transform transform{};
				entity.AddComponent(transform);

				Image image{};
				entity.AddComponent(image);

				Animator animator{};
				entity.AddComponent(animator);

				Direction direction{};
				entity.AddComponent(direction);

				SpwanTimer spwanTimer{};
				entity.AddComponent(spwanTimer);

				DamageTimer damageTimer{};
				entity.AddComponent(damageTimer);

				Hp hp{};
				entity.AddComponent(hp);

				Active active{};
				entity.AddComponent(active);

				Color color{};
				entity.AddComponent(color);

				CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
				collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
				entity.AddComponent(collider);

				BoxCollider boxCollider{};
				boxCollider.size = { .width = float(mArcherRunTextures[2].GetWidth()), .height = float(mArcherRunTextures[2].GetHeight()) };
				entity.AddComponent(boxCollider);

				GetEntityWorld()->AddEntity(&entity);
			}
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

void MainScene::playerState(const float deltaTime)
{
	Player* player = mPlayer.GetComponent<Player>();
	player->state = (player->length != 0.0f) ? Player::eState::Run : Player::eState::Idle;

	static float deadTimer;
	Hp* playerHp = mPlayer.GetComponent<Hp>();
	if (playerHp->value <= 0)
	{
		deadTimer += deltaTime;
		if (deadTimer >= 0.5f)
		{
			player->state = Player::eState::Dead;
			Label* label = mPlayer.GetComponent<Label>();
			label->active = false;
		}
	}
}

void MainScene::playerMove(const float deltaTime)
{
	const int32_t moveX = Input::Get().GetKey(SDL_SCANCODE_D) - Input::Get().GetKey(SDL_SCANCODE_A);
	const int32_t moveY = Input::Get().GetKey(SDL_SCANCODE_W) - Input::Get().GetKey(SDL_SCANCODE_S);

	static int32_t prevMoveX;
	static int32_t prevMoveY;

	static Point moveVelocity = {};
	constexpr float MAX_SPEED = 300.0f;
	constexpr float ACC = 60.0f;

	if (moveX != 0)
	{
		moveVelocity.x = std::clamp(moveVelocity.x + ACC * moveX, -MAX_SPEED, MAX_SPEED);
		prevMoveX = moveX;
	}
	else
	{
		if (prevMoveX > 0)
		{
			moveVelocity.x = std::max(moveVelocity.x - ACC, 0.0f);
		}
		else
		{
			moveVelocity.x = std::min(moveVelocity.x + ACC, 0.0f);
		}
	}

	if (moveY != 0)
	{
		moveVelocity.y = std::clamp(moveVelocity.y + ACC * moveY, -MAX_SPEED, MAX_SPEED);
		prevMoveY = moveY;
	}
	else
	{
		if (prevMoveY > 0)
		{
			moveVelocity.y = std::max(moveVelocity.y - ACC, 0.0f);
		}
		else
		{
			moveVelocity.y = std::min(moveVelocity.y + ACC, 0.0f);
		}
	}

	Player* player = mPlayer.GetComponent<Player>();
	player->length = Math::GetVectorLength(moveVelocity);

	Point moveDirection = Math::NormalizeVector(moveVelocity);
	float velocity = fmin(Math::GetVectorLength(moveVelocity), MAX_SPEED);
	moveVelocity = moveDirection * velocity;

	constexpr float KNOCKBACK_FORCE = 500.0f;
	constexpr float KNOCKBACK_COOLTIMER = 0.7f;
	Knockback* knockback = mPlayer.GetComponent<Knockback>();
	Color* color = mPlayer.GetComponent<Color>();

	if (knockback->isValue)
	{
		color->r = 200;
		color->g = 0;
		color->b = 0;

		moveVelocity = moveVelocity + knockback->direction * KNOCKBACK_FORCE;

		knockback->coolTimer += deltaTime;
		if (knockback->coolTimer >= KNOCKBACK_COOLTIMER)
		{
			color->r = 255;
			color->g = 255;
			color->b = 255;

			knockback->isValue = false;
			knockback->coolTimer = 0.0f;
		}
	}

	Transform* transform = mPlayer.GetComponent<Transform>();
	clampToTile(transform, { .x = 5.0f, .xx = 5.0f }, { .y = -8.0f, .yy = 50.0f });
	transform->position = transform->position + moveVelocity * deltaTime;

	Direction* direction = mPlayer.GetComponent<Direction>();
	direction->value = Math::NormalizeVector(getScreenMousePosition() - transform->position);

	if (direction->value.x > 0.0f)
	{
		transform->flip = SDL_FLIP_NONE;
	}
	else if (direction->value.x < 0.0f)
	{
		transform->flip = SDL_FLIP_HORIZONTAL;
	}
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

	case Player::eState::Dead:
		animator->SetClip(&mPlayerClips[uint32_t(Player::eState::Dead)]);
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
			and not active->isValue
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
				active->isValue = true;

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
	const Clip& attackClip = desc.attackClip;
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

		if (monster->state == Monster::eState::Spwan)
		{
			spwan->spwanWaitingTimer += deltaTime;

			float waitingTime = spwanWaitingTime / 2.0f;
			if (spwan->spwanWaitingTimer >= waitingTime)
			{
				spwan->spwanBlinkTimer += deltaTime;
				if (spwan->spwanBlinkTimer >= 0.06f)
				{
					active->isValue = !active->isValue;
					spwan->spwanBlinkTimer = 0.0f;
				}
			}
			if (spwan->spwanWaitingTimer >= spwanWaitingTime)
			{
				monsterTransform->scale = { .width = originScale, .height = originScale };

				monster->state = Monster::eState::Run;
				active->isValue = true;
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
			if (monster->state != Monster::eState::None
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

			monster->state = Monster::eState::None;
			Active* monsterActive = entity.GetComponent<Active>();
			monsterActive->isValue = false;

			for (Entity& entity : mDeadParticle)
			{
				if (not monsterActive->isValue
					and spwan->isSpwan)
				{
					Active* particleActive = entity.GetComponent<Active>();
					particleActive->isValue = true;

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
					active->isValue = false;
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
		Knockback* playerKnockback = mPlayer.GetComponent<Knockback>();

		const Point monsterPosition = monsterTransform->position;
		const Point playerPosition = playerTransform->position;
		const Point difference = playerPosition - monsterPosition;
		monster->length = Math::GetVectorLength(difference);

		Direction* direction = entity.GetComponent<Direction>();
		if (monster->state == Monster::eState::Run)
		{
			direction->value = Math::NormalizeVector(difference);
			velocity = direction->value * maxSpeed;
			playerKnockback->direction = direction->value;

			clampToTile(monsterTransform, { .x = 5.0f, .xx = 5.0f }, { .y = -8.0f, .yy = 50.0f });
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
		case Monster::eState::None:
			__noop;
			break;

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

		default:
			assert(false and "지원하지 않는 애니메이션입니다.");
			break;
		}
	}
}

void MainScene::clampToTile(Transform* transform, const RangeX offsetX, const RangeY offsetY)
{
	assert(transform != nullptr);

	const Point firstTilePosition = mTiles[0][0].GetComponent<Transform>()->position;
	const Point lastTilePosition = mTiles[mTileMaxCount - 1][mTileMaxCount - 1].GetComponent<Transform>()->position;

	transform->position.x = std::clamp(transform->position.x, firstTilePosition.x + offsetX.x, lastTilePosition.x - offsetX.xx);
	transform->position.y = std::clamp(transform->position.y, lastTilePosition.y + offsetY.y, firstTilePosition.y - offsetY.yy);
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
	const float dgreeOffset = desc.dgreeOffset;
	const SDL_RendererFlip flipX = desc.flipX;
	const SDL_RendererFlip flipY = desc.flipY;

	Transform* transform = weaponEntity->GetComponent<Transform>();
	Direction* direction = weaponEntity->GetComponent<Direction>();

	const Transform* playerTransform = mPlayer.GetComponent<Transform>();
	const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;
	float degree = std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);
	degree = degree + dgreeOffset;

	constexpr Point OFFSET =
	{
		.x = 3.0f,
		.y = 20.0f
	};

	transform->position = playerTransform->position + OFFSET;
	transform->angle = -degree;

	const float length = Math::GetVectorLength(mouseToPlayer);
	direction->value = mouseToPlayer / length;
}

float MainScene::getRandom(const float min, const float max)
{
	const float result = float(rand()) / RAND_MAX * (max - min) + min;
	return result;
}

bool MainScene::isCollisionEnter(const Entity& entity0, const Entity& entity1) const
{
	std::pair<const Entity*, const Entity*> collidedEntityPair = getCollidedEntityPair(entity0, entity1);

	if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
		foundCollidedEntityPair != mCollidedEntityPairs.end())
	{
		const auto& foundPreviousCollidedEntityPair = std::find(mPreviousCollidedEntityPairs.begin(), mPreviousCollidedEntityPairs.end(), collidedEntityPair);
		return foundPreviousCollidedEntityPair == mPreviousCollidedEntityPairs.end();
	}

	return false;
}

bool MainScene::isCollisionStay(const Entity& entity0, const Entity& entity1) const
{
	std::pair<const Entity*, const Entity*> collidedEntityPair = getCollidedEntityPair(entity0, entity1);

	const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
	return foundCollidedEntityPair != mCollidedEntityPairs.end();
}

bool MainScene::isCollisionExit(const Entity& entity0, const Entity& entity1) const
{
	std::pair<const Entity*, const Entity*> collidedEntityPair = getCollidedEntityPair(entity0, entity1);

	if (const auto& foundPreviousCollidedEntityPair = std::find(mPreviousCollidedEntityPairs.begin(), mPreviousCollidedEntityPairs.end(), collidedEntityPair);
		foundPreviousCollidedEntityPair != mPreviousCollidedEntityPairs.end())
	{
		const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
		return foundCollidedEntityPair == mCollidedEntityPairs.end();
	}

	return false;
}

std::pair<const Entity*, const Entity*> MainScene::getCollidedEntityPair(const Entity& entity0, const Entity& entity1) const
{
	std::pair<const Entity*, const Entity*> collidedEntityPair{};
	if (&entity0 < &entity1)
	{
		collidedEntityPair = { &entity0, &entity1 };
	}
	else
	{
		collidedEntityPair = { &entity1, &entity0 };
	}

	return collidedEntityPair;
}

void MainScene::registerCollidedEntityPairs(const Entity& entity0, const Entity& entity1)
{
	std::pair<const Entity*, const Entity*> colliderEntityPair = getCollidedEntityPair(entity0, entity1);

	if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), colliderEntityPair);
		foundCollidedEntityPair == mCollidedEntityPairs.end())
	{
		mCollidedEntityPairs.push_back(colliderEntityPair);
	}
}

Rect MainScene::convertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const
{
	const Point position = transform.position + boxCollider.offset;
	const Scale boxHalfSize = transform.scale * boxCollider.size * 0.5f;

	const Rect result
	{
		.left = position.x - boxHalfSize.width,
		.top = position.y + boxHalfSize.height,
		.right = position.x + boxHalfSize.width,
		.bottom = position.y - boxHalfSize.height,
	};

	return result;
}

Circle MainScene::convertCircleColliderToWorldCircle(const Transform& transform, const CircleCollider& circleCollider) const
{
	const Circle result =
	{
		.center = transform.position + circleCollider.offset,
		.radius = transform.scale.width * circleCollider.radius
	};

	return result;
}

Line MainScene::convertLineColliderToWorldLine(const Transform& transform, const LineCollider& lineCollider) const
{
	const Point position = transform.position + lineCollider.offset;
	const Scale boxHalfSize = transform.scale * lineCollider.scale * 0.5f;

	const Rect rect
	{
		.left = position.x - boxHalfSize.width,
		.top = position.y + boxHalfSize.height,
		.right = position.x + boxHalfSize.width,
		.bottom = position.y - boxHalfSize.height,
	};

	float centerY = rect.top + (transform.scale.height * lineCollider.scale.height * 0.5f);

	const Line result =
	{
		.point0 = {.x = rect.left, .y = rect.top },
		.point1 = {.x = rect.right, .y = rect.bottom }
	};

	return result;
}

bool MainScene::checkCollisionBoxBox(const Entity& entity0, const Entity& entity1)
{
	if (not entity0.HasComponent<BoxCollider>()
		or not entity1.HasComponent<BoxCollider>())
	{
		return false;
	}

	const Transform* transform0 = entity0.GetComponent<Transform>();
	const BoxCollider* boxCollider0 = entity0.GetComponent<BoxCollider>();
	const Rect rect0 = convertBoxColliderToWorldBox(*transform0, *boxCollider0);

	const Transform* transform1 = entity1.GetComponent<Transform>();
	const BoxCollider* boxCollider1 = entity1.GetComponent<BoxCollider>();
	const Rect rect1 = convertBoxColliderToWorldBox(*transform1, *boxCollider1);

	if (Collision::IsCollidedSqureWithSqure(rect0, rect1))
	{
		registerCollidedEntityPairs(entity0, entity1);
		return true;
	}

	return false;
}

bool MainScene::checkCollisionBoxCircle(const Entity& boxEntity, const Entity& circleEntity)
{
	if (not boxEntity.HasComponent<BoxCollider>()
		or not circleEntity.HasComponent<CircleCollider>())
	{
		return false;
	}

	const Transform* boxTransform = boxEntity.GetComponent<Transform>();
	const BoxCollider* boxCollider = boxEntity.GetComponent<BoxCollider>();
	const Rect rect = convertBoxColliderToWorldBox(*boxTransform, *boxCollider);

	const Transform* circleTransform = circleEntity.GetComponent<Transform>();
	const CircleCollider* circleCollider = circleEntity.GetComponent<CircleCollider>();
	const Circle circle = convertCircleColliderToWorldCircle(*circleTransform, *circleCollider);

	if (Collision::IsCollidedSqureWithCircle(rect, circle))
	{
		registerCollidedEntityPairs(boxEntity, circleEntity);
		return true;
	}

	return false;
}

bool MainScene::checkCollisionBoxLine(const Entity& boxEntity, const Entity& lineEntity)
{
	if (not boxEntity.HasComponent<BoxCollider>()
		or not lineEntity.HasComponent<LineCollider>())
	{
		return false;
	}

	const Transform* boxTransform = boxEntity.GetComponent<Transform>();
	const BoxCollider* boxCollider = boxEntity.GetComponent<BoxCollider>();
	const Rect rect = convertBoxColliderToWorldBox(*boxTransform, *boxCollider);

	const Transform* lineTransform = lineEntity.GetComponent<Transform>();
	const LineCollider* lineCollider = lineEntity.GetComponent<LineCollider>();
	const Line line = convertLineColliderToWorldLine(*lineTransform, *lineCollider);

	if (Collision::IsCollidedSqureWithLine(rect, line))
	{
		registerCollidedEntityPairs(boxEntity, lineEntity);
		return true;
	}

	return false;
}

bool MainScene::checkCollisionCircleCircle(const Entity& entity0, const Entity& entity1)
{
	if (not entity0.HasComponent<CircleCollider>()
		or not entity1.HasComponent<CircleCollider>())
	{
		return false;
	}

	const Transform* Transform0 = entity0.GetComponent<Transform>();
	const CircleCollider* Collider0 = entity0.GetComponent<CircleCollider>();
	const Circle circle0 = convertCircleColliderToWorldCircle(*Transform0, *Collider0);

	const Transform* Transform1 = entity1.GetComponent<Transform>();
	const CircleCollider* Collider1 = entity1.GetComponent<CircleCollider>();
	const Circle circle1 = convertCircleColliderToWorldCircle(*Transform1, *Collider1);

	if (Collision::IsCollidedCircleWithCircle(circle0, circle1))
	{
		registerCollidedEntityPairs(entity0, entity1);
		return true;
	}

	return false;
}

bool MainScene::checkCollisionCircleLine(const Entity& circleEntity, const Entity& lineEntity)
{
	if (not circleEntity.HasComponent<CircleCollider>()
		or not lineEntity.HasComponent<LineCollider>())
	{
		return false;
	}

	const Transform* circleTransform = circleEntity.GetComponent<Transform>();
	const CircleCollider* circleCollider = circleEntity.GetComponent<CircleCollider>();
	const Circle circle = convertCircleColliderToWorldCircle(*circleTransform, *circleCollider);

	const Transform* lineTransform = lineEntity.GetComponent<Transform>();
	const LineCollider* lineCollider = lineEntity.GetComponent<LineCollider>();
	const Line line = convertLineColliderToWorldLine(*lineTransform, *lineCollider);

	if (Collision::IsCollidedCircleWithLine(circle, line))
	{
		registerCollidedEntityPairs(circleEntity, lineEntity);
		return true;
	}

	return false;
}