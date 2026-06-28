#include "pch.h"

#include "Core/Collision.h"
#include "Core/Constant.h"
#include "Core/Helper.h"
#include "Core/Input.h"

#include "MainScene.h"

constexpr float PRIMARY_SIZE = 3.0f;

void MainScene::Initialize()
{
	initialize_Resource();
	initialize_Entity();

	initializePlayer();

	initializeSword();
	initializeSwordSkill();

	initializeGun();
	initializeBullets();

	initializeUI();

	initializeWaveTimer();
	initializeWaveStage();
}

bool MainScene::Update(const float deltaTime)
{
	// Core
	{
		for (const Entity* entity0 : GetEntityWorld()->GetAllEntities())
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

			for (const Entity* entity1 : GetEntityWorld()->GetAllEntities())
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

				if (entity0->HasComponent<Active>()
					or entity1->HasComponent<Active>())
				{
					const Active* active0 = entity0->GetComponent<Active>();
					const Active* active1 = entity1->GetComponent<Active>();
					if (not active0->isValue
						or not active1->isValue)
					{
						continue;
					}
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

	updateCamera();
	input();

	// Wave를 업데이트한다.
	{
		// 현재 웨이브 상태를 업데이트한다.
		// 다음 웨이브를 위해 값을 초기화한다.
		mGameWaveState.waveTimer -= deltaTime;
		if (mGameWaveState.waveTimer <= 0.0f)
		{
			if (++mGameWaveState.index >= WAVES.size())
			{
				assert(false and "구현 예정.");
			}

			mGameWaveState.waveTimer = WAVES[mGameWaveState.index].elapsedTime;
			mGameWaveState.groupIndex = 0;
			mGameWaveState.remainingMonsterGroupSpawnTimer = WAVES[mGameWaveState.index].monsterGroupSpawnIntervalTime;
			mGameWaveState.labelShowElapsedTimer = 0.0f;

			mMonsterIndex = 0;

			// 몬스터를 갱신한다.
			for (const std::vector<Entity*> monsterEntities = getEntities<MonsterTag>(); 
				Entity* entity : monsterEntities)
			{
				Monster* monster = entity->GetComponent<Monster>();
				monster->state = Monster::eState::Dead;

				entity->SetRemove();
			}

			// 몬스터 체력바를 갱신한다.
			for (const std::vector<Entity*> hpBarEntities = getEntities<MonsterHpBarTag>();
				Entity* entity : hpBarEntities)
			{
				entity->SetRemove();
			}

			// 몬스터 충돌체를 갱신한다.
			for (const std::vector<Entity*> attackColliders = getEntities<MonsterAttackColliderTag>();
			Entity* entity : attackColliders)
			{
				entity->SetRemove();
			}

			// 플레이어 총알을 갱신한다.
			{
				const Entity* bulletCountEntity = getEntity<BulletCountTag>();
				Label* label = bulletCountEntity->GetComponent<Label>();
				label->text = std::to_string(mBulletState.count) + "/" + std::to_string(mBulletState.maxCount);
			}

			// 라벨 정보를 갱신한다.
			{
				const Entity* entity = getEntity<WaveStageTag>();
;				Label* label = entity->GetComponent<Label>();
				const std::string name = std::to_string(mGameWaveState.index + 1) + " Wave";
				label->text = name;

				Active* active = entity->GetComponent<Active>();
				active->isValue = true;
			}
		}

		{
			const WaveDesc& waveDesc = WAVES[mGameWaveState.index];
			const uint32_t monsterGroupIndex = waveDesc.monsterGroupIndicies[mGameWaveState.groupIndex];
			const MonsterGroup& monsterGroup = MONSTER_GROUPS[monsterGroupIndex];

			mGameWaveState.remainingMonsterGroupSpawnTimer -= deltaTime;
			if (mGameWaveState.remainingMonsterGroupSpawnTimer <= 0.0f)
			{
				spawnMonsterGroup(monsterGroup);

				++mGameWaveState.groupIndex;
				assert(mGameWaveState.groupIndex < MONSTER_GROUPS.size() && "더 이상 그룹이 없습니다.");

				mGameWaveState.remainingMonsterGroupSpawnTimer = waveDesc.monsterGroupSpawnIntervalTime;
			}
		}

		// 다음 웨이브가 되면 잠시 동안 라벨을 표시한다.
		{
			mGameWaveState.labelShowElapsedTimer += deltaTime;

			const Entity* entity = getEntity<WaveStageTag>();
			Active* active = entity->GetComponent<Active>();
			if (active->isValue and mGameWaveState.labelShowElapsedTimer >= 2.0f)
			{
				mGameWaveState.labelShowElapsedTimer = 0.0f;
				active->isValue = false;
			}
		}

		// Upeate Wave Timer Label
		{
			const Entity* entity = getEntity<WaveTimerTag>();
			Label* label = entity->GetComponent<Label>();
			const uint32_t seconds = uint32_t(mGameWaveState.waveTimer) % 60;
			const uint32_t minutes = uint32_t(mGameWaveState.waveTimer) / 60;

			const std::string fseconds = (seconds >= 10) ? std::to_string(seconds) : "0" + std::to_string(seconds);
			const std::string fMinutes = (minutes >= 10) ? std::to_string(minutes) : "0" + std::to_string(minutes);

			const std::string name = "Timer: " + fMinutes + ":" + fseconds;
			label->text = name;
		}
	}

	// 플레이어를 업데이트한다.
	playerState(deltaTime);
	playerMove(deltaTime);

	updateSword();
	updateSwordStates(deltaTime);

	updateGun();

	spawnSwordSkill();
	updateSwordSkill(deltaTime);
	updateSwordSkillStates(deltaTime);

	spawnBullets(deltaTime);
	updateBullets(deltaTime);
	updateBulletStates(deltaTime);

	// 몬스터를 업데이트한다.
	updateMonsterStates(deltaTime);
	updateHpMonsters(deltaTime);
	monsterMove(deltaTime);
	monsterHpBarMove();

	// 원거리 몬스터의 공격을 초기화와 업데이트한다.
	spawnRangedAttack<MonsterArrowTag>
	(
		SpawnRangeAttackDesc
		{
			.type = eMonsterType::Archer,
			.texture = &mArrowTexture,
			.spawnFrameIndex = 7
		}
	);
	rangedAttackState<MonsterArrowTag>();
	rangedAttackMove(300.0f, deltaTime);

	// 보스를 업데이트한다.
	updateBossStates(deltaTime);

	// 충돌을 업데이트한다.
	{
		spawnAttackCollider();
		updateAttackCollision();
		removeAttackCollider();

		playerToMonsterCollision();
		playerToMonsterAttackCollision();
		playerToArrowCollision();

		//swordSkillToMonsterCollision();
		//bulletToMonsterCollision();

		mPreviousCollidedEntityPairs = mCollidedEntityPairs;
		mCollidedEntityPairs.clear();
	}

	playerSetClip();
	monsterSetClip();
	BossSetClip();

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

		mPlayerShadowTexture.Finalize();
		mPlayerHandTexture.Finalize();
		mPlayerDeadTexture.Finalize();
	}

	// Monster
	{
		mArrowTexture.Finalize();

		for (Texture& texture : mSpwanTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mBigWhiteSkelRunTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mBigWhiteSkelAttackTextures)
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

		for (Texture& texture : mSkelDogIdleTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mSkelDogRunTextures)
		{
			texture.Finalize();
		}

		mSkelDogAttackTexture.Finalize();

		for (Texture& texture : mBossIdleTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mBossAttackTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mBossLeftHandTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mBossBackTextures)
		{
			texture.Finalize();
		}

		for (Texture& texture : mCycloneFanTextures)
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

	mRedRectTexture.Finalize();
}

void MainScene::initialize_Resource()
{
	mUIFont.Initilize("Resource/DroidSans.TTF", 50);
	mHpFont.Initilize("Resource/DroidSans.TTF", 20);

	// Tile
	mTileTextures[0].Initialize(GetHelper(), "Resource/Tile/0.png");
	mTileTextures[1].Initialize(GetHelper(), "Resource/Tile/1.png");

	mRedRectTexture.Initialize(GetHelper(), "Resource/RedRectangle.png");

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

		mPlayerShadowTexture.Initialize(GetHelper(), "Resource/Char/Alice/Dash/0.png");
		mPlayerHandTexture.Initialize(GetHelper(), "Resource/Char/Alice/Hand/0.png");

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
		uint32_t index{};
		for (Texture& texture : mBulletTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Gun/Bullet/" + std::to_string(index++) + ".png");

			Clip::Frame frame =
			{
				.texture = &texture,
				.durationTime = 0.02f
			};

			mBulletClip.AddClip(frame);
		}
	}

	uint32_t index{};

	// Big White
	{
		// Spwan
		for (Texture& texture : mSpwanTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Monster/Spawn/" + std::to_string(index++) + ".png");

			Clip::Frame frame =
			{
				.texture = &texture,
				.durationTime = 0.06f
			};

			mBigWhiteSkelClips[uint32_t(Monster::eState::Spawn)].AddClip(frame);
		}

		index = 0;

		// Run
		{
			constexpr Point RUN_OFFSETS[] =
			{
				{.x = 0.07f, .y = -0.5f },
				{.x = 0.07f, .y = -0.5f },
				{.x = 0.07f, .y = -0.5f },
				{.x = 0.07f, .y = -0.5f },
				{.x = 0.07f, .y = -0.5f },
				{.x = 0.07f, .y = -0.5f },
			};

			assert(std::size(RUN_OFFSETS) == mBigWhiteSkelRunTextures.size() and "offset 개수와 texture 개수가 일치하지 않습니다.");

			for (Texture& texture : mBigWhiteSkelRunTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/BigWhiteSekl/Run/" + std::to_string(index) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.14f,
					.center = RUN_OFFSETS[index]
				};

				mBigWhiteSkelClips[uint32_t(Monster::eState::Run)].AddClip(frame);
				++index;
			}

			index = 0;
		}

		// Attack
		{
			constexpr Point ATTACK_OFFSETS[] =
			{
				{.x = 0.13f, .y = -0.5f },
				{.x = 0.08f, .y = -0.5f },
				{.x = -0.24f,.y = -0.5f },
				{.x = -0.34f,.y = -0.5f },
				{.x = -0.34f,.y = -0.5f },
				{.x = -0.35f,.y = -0.5f },
				{.x = -0.30f,.y = -0.5f },
				{.x = -0.28f,.y = -0.5f },
				{.x = -0.28f,.y = -0.5f },
				{.x = -0.16f,.y = -0.5f },
				{.x = -0.14f,.y = -0.5f },
				{.x = 0.11f, .y = -0.5f }
			};

			assert(std::size(ATTACK_OFFSETS) == mBigWhiteSkelAttackTextures.size() and "offset 개수와 texture 개수가 일치하지 않습니다.");

			for (Texture& texture : mBigWhiteSkelAttackTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/BigWhiteSekl/Attack/" + std::to_string(index) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.1f,
					.center = ATTACK_OFFSETS[index]
				};

				mBigWhiteSkelClips[uint32_t(Monster::eState::Attack)].AddClip(frame);
				++index;
			}

			index = 0;
		}
	}

	// Archer
	{
		// Spwan
		{
			for (Texture& texture : mSpwanTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Spawn/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.06f
				};

				mArcherClips[uint32_t(Monster::eState::Spawn)].AddClip(frame);
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

		mArrowTexture.Initialize(GetHelper(), "Resource/Monster/Archer/Arrow/0.png");
	}

	// Skel
	{
		// Spwan
		{
			for (Texture& texture : mSpwanTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Spawn/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.06f
				};

				mSkelDogClips[uint32_t(Monster::eState::Spawn)].AddClip(frame);
			}

			index = 0;
		}

		// Idle
		{
			for (Texture& texture : mSkelDogIdleTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/SkelDog/Idle/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};

				mSkelDogClips[uint32_t(Monster::eState::Idle)].AddClip(frame);
			}

			index = 0;
		}

		// Run
		{
			for (Texture& texture : mSkelDogRunTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/SkelDog/Run/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};

				mSkelDogClips[uint32_t(Monster::eState::Run)].AddClip(frame);
			}

			index = 0;
		}

		// Attack
		{
			mSkelDogAttackTexture.Initialize(GetHelper(), "Resource/Monster/SkelDog/Attack/0.png");

			Clip::Frame frame =
			{
				.texture = &mSkelDogAttackTexture,
				.durationTime = 0.0f,
			};

			mSkelDogClips[uint32_t(Monster::eState::Attack)].AddClip(frame);
		}
	}

	// Boss
	{
		// Spwan
		{
			for (Texture& texture : mSpwanTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Spawn/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.06f
				};

				mBossClips[uint32_t(Monster::eState::Spawn)].AddClip(frame);
			}

			index = 0;
		}

		// Idle
		{
			for (Texture& texture : mBossIdleTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Boss/Idle/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};

				mBossClips[uint32_t(Monster::eState::Idle)].AddClip(frame);
			}

			index = 0;
		}

		// Run
		{
			mBossIdleTextures[0].Initialize(GetHelper(), "Resource/Monster/Boss/Idle/0.png");

			Clip::Frame frame =
			{
				.texture = &mBossIdleTextures[0],
				.durationTime = 0.12f,
			};

			mBossClips[uint32_t(Monster::eState::Run)].AddClip(frame);

			index = 0;
		}

		// Attack
		{
			for (Texture& texture : mBossAttackTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Boss/Attack/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};

				mBossClips[uint32_t(Monster::eState::Attack)].AddClip(frame);
			}

			index = 0;
		}

		// Left Hand
		{
			for (Texture& texture : mBossLeftHandTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Boss/Hand/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};

				mBossLeftHandClip.AddClip(frame);
			}

			index = 0;
		}

		// Right Hand
		{
			for (Texture& texture : mBossLeftHandTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Boss/Hand/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};

				mBossRightHandClip.AddClip(frame);
			}

			index = 0;
		}

		// Boss Back
		{
			for (Texture& texture : mBossBackTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Boss/Back/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};

				mBossBackClip.AddClip(frame);
			}

			index = 0;
		}

		// Boss CycloneFan
		{
			for (Texture& texture : mCycloneFanTextures)
			{
				texture.Initialize(GetHelper(), "Resource/Monster/Boss/CycloneFan/" + std::to_string(index++) + ".png");

				Clip::Frame frame =
				{
					.texture = &texture,
					.durationTime = 0.12f,
				};

				mCycloneFanClip.AddClip(frame);
			}

			index = 0;
		}
	}
}

void MainScene::initialize_Entity()
{
	// Camera
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(Camera());
		entity->AddComponent(Transform());
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
		// Timer
		{
			Entity* entity = GetEntityWorld()->AddEntity(new Entity());
			entity->AddComponent(WaveTimerTag());
			entity->AddComponent(Ui());
			entity->AddComponent(Label());
			entity->AddComponent(Transform());
			entity->AddComponent(Active());
		}

		// Wave Stage
		{
			Entity* entity = GetEntityWorld()->AddEntity(new Entity());
			entity->AddComponent(WaveStageTag());
			entity->AddComponent(Ui());
			entity->AddComponent(Label());
			entity->AddComponent(Transform());
			entity->AddComponent(Active());
		}

		// Bullet Count
		{
			Entity* entity = GetEntityWorld()->AddEntity(new Entity());
			entity->AddComponent(BulletCountTag());
			entity->AddComponent(Ui());
			entity->AddComponent(Label());
			entity->AddComponent(Transform());
			entity->AddComponent(Active());
		}
	}

	// Gun
	{
		constexpr Point CENTER = { .x = -0.25f,.y = -0.25f };

		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(GunTag());
		entity->AddComponent(Direction());
		entity->AddComponent(Transform());
		entity->AddComponent(Image());
		entity->AddComponent(Active());
		entity->AddComponent(Color());
	}

	// Dash Shadow
	{
		for (std::array<Entity*, 5> entities{}; 
			Entity* entity : entities)
		{
			Entity* newEntity = GetEntityWorld()->AddEntity(new Entity());
			newEntity->AddComponent(ShadowTag());
			newEntity->AddComponent(Shadow());
			newEntity->AddComponent(Transform());
			newEntity->AddComponent(Image());
			newEntity->AddComponent(Active());
			newEntity->AddComponent(Color());
		}
	}

	// Player Left Hand
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(LeftHandTag());
		entity->AddComponent(Transform());
		entity->AddComponent(Image());
		entity->AddComponent(Active());
	}

	// Player
	{
		constexpr uint32_t MAX_HP = 100;

		mPlayerClips[uint32_t(Player::eState::Idle)].SetLoop(true);
		mPlayerClips[uint32_t(Player::eState::Run)].SetLoop(true);

		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(PlayerTag());
		entity->AddComponent(Player());
		entity->AddComponent(Image());
		entity->AddComponent(Direction());
		entity->AddComponent(Transform());
		entity->AddComponent(Animator());
		entity->AddComponent(Hp());
		entity->AddComponent(Dash());
		entity->AddComponent(Knockback());
		entity->AddComponent(Active());
		entity->AddComponent(Color());
		entity->AddComponent(BoxCollider());
		entity->AddComponent(DebugActive());
		entity->AddComponent(DebugColor());

		CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Player));
		collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Monster));
		entity->AddComponent(collider);
	}

	// Player Hp
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(PlayerHpBarTag());
		entity->AddComponent(Ui());
		entity->AddComponent(Label());
		entity->AddComponent(Transform());
		entity->AddComponent(Active());
		entity->AddComponent(Color());
	}

	// Player Right Hand
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(RightHandTag());
		entity->AddComponent(Transform());
		entity->AddComponent(Image());
		entity->AddComponent(Active());
	}

	// Sword
	{
		mSwordClip.SetLoop(true);

		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(SwordTag());
		entity->AddComponent(Direction());
		entity->AddComponent(Transform());
		entity->AddComponent(Image());
		entity->AddComponent(Animator());
		entity->AddComponent(Active());
		entity->AddComponent(Color());
	}

	// Sword Skill
	{
		const float SIZE = 2.5f;

		mSwordSkillClip.SetLoop(true);

		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(SwordSkillTag());
		entity->AddComponent(Effect());
		entity->AddComponent(RangedAttack());
		entity->AddComponent(Direction());
		entity->AddComponent(Transform());
		entity->AddComponent(Image());
		entity->AddComponent(Animator());
		entity->AddComponent(Active());
		entity->AddComponent(Color());
		entity->AddComponent(BoxCollider());
		entity->AddComponent(DebugActive());
		entity->AddComponent(DebugColor());

		CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::SwordSkill));
		collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Monster));
		entity->AddComponent(collider);
	}

	// Bullet
	{
		mBulletClip.SetLoop(true);

		for (const std::array<Entity*, 10> entities{}; 
			Entity* entity : entities)
		{
			Entity* newEntity = GetEntityWorld()->AddEntity(new Entity());
			newEntity->AddComponent(BulletTag());
			newEntity->AddComponent(RangedAttack());
			newEntity->AddComponent(Direction());
			newEntity->AddComponent(Transform());
			newEntity->AddComponent(Image());
			newEntity->AddComponent(Animator());
			newEntity->AddComponent(Active());
			newEntity->AddComponent(Color());
			newEntity->AddComponent(BoxCollider());
			newEntity->AddComponent(DebugActive());
			newEntity->AddComponent(DebugColor());

			CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Bullet));
			collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Monster));
			newEntity->AddComponent(collider);
		}

		const Entity* bulletCountEntity = getEntity<BulletCountTag>();
		Label* label = bulletCountEntity->GetComponent<Label>();
		label->text = std::to_string(mBulletState.maxCount) + "/" + std::to_string(mBulletState.maxCount);
	}

	// Monster
	{
		mBigWhiteSkelClips[uint32_t(Monster::eState::Spawn)].SetLoop(true);
		mBigWhiteSkelClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mBigWhiteSkelClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		mArcherClips[uint32_t(Monster::eState::Spawn)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		mSkelDogClips[uint32_t(Monster::eState::Spawn)].SetLoop(true);
		mSkelDogClips[uint32_t(Monster::eState::Idle)].SetLoop(true);
		mSkelDogClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mSkelDogClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		mBossClips[uint32_t(Monster::eState::Spawn)].SetLoop(true);
		mBossClips[uint32_t(Monster::eState::Idle)].SetLoop(true);
		mBossClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mBossClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		mBossLeftHandClip.SetLoop(true);
		mBossRightHandClip.SetLoop(true);

		mBossBackClip.SetLoop(true);

		mCycloneFanClip.SetLoop(true);

		mCycloneFanState =
		{
			.maxCount = 10,
			.count = 10,
			.fireTimer = 0.0f,
			.reloadTimer = 0.0f
		};
	}
}

void MainScene::input()
{
	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
	{
		mIsUpdate = false;
	}

#if defined(_DEBUG)
	if (Input::Get().GetKeyDown(SDL_SCANCODE_T))
	{
		mIsDebugActive = !mIsDebugActive;

		for (const Entity* entity : GetEntityWorld()->GetAllEntities())
		{
			if (not entity->HasComponent<DebugActive>())
			{
				continue;
			}

			DebugActive* debugActive = entity->GetComponent<DebugActive>();
			debugActive->isValue = mIsDebugActive;
		}

		for (const std::vector<Entity*> entities = getEntities<MonsterAttackColliderTag>();
			Entity* attackEntity : entities)
		{
			if (not attackEntity->HasComponent<DebugActive>())
			{
				continue;
			}

			DebugActive* debugActive = attackEntity->GetComponent<DebugActive>();
			debugActive->isValue = !debugActive->isValue;
		}
	}
#endif
}

void MainScene::updateCamera()
{
	constexpr Point OFFSET = { .x = 24.0f, .y = 23.0f };

	const Entity* entity = getEntity<Camera>();
	Transform* transform = entity->GetComponent<Transform>();

	const Entity* playerEntity = getEntity<PlayerTag>();
	Transform* target = playerEntity->GetComponent<Transform>();
	
	transform->position = target->position;

	const Scale halfScreen =
	{
		.width = Constant::Get().GetHalfWidth() - OFFSET.x,
		.height = Constant::Get().GetHalfHeight() - OFFSET.y,
	};

	clampToTile(transform, { .min = halfScreen.width, .max = halfScreen.width }, { .min = halfScreen.height, .max = halfScreen.height });
}

Rect& MainScene::getCameraRect() const
{
	constexpr Point OFFSET = { .x = 24.0f, .y = 23.0f };

	const Scale halfScreen =
	{
		.width = Constant::Get().GetHalfWidth() - OFFSET.x,
		.height = Constant::Get().GetHalfHeight() - OFFSET.y,
	};

	const Entity* entity = getEntity<Camera>();
	Transform* transform = entity->GetComponent<Transform>();

	Rect result =
	{
		.left = transform->position.x - halfScreen.width,
		.top = transform->position.y + halfScreen.height,
		.right = transform->position.x + halfScreen.width,
		.bottom = transform->position.y - halfScreen.height
	};

	return result;
}

void MainScene::initializeUI()
{
	// Player Hp
	{
		const Entity* entity = getEntity<PlayerHpBarTag>();

		Label* label = entity->GetComponent<Label>();
		label->font = &mUIFont;

		const Entity* playerEntity = getEntity<PlayerTag>();

		const Hp* hp = playerEntity->GetComponent<Hp>();
		std::string hpName = "HP: " + std::to_string(hp->value);
		label->text = hpName;

		Active* active = entity->GetComponent<Active>();
		active->isValue = true;
	}

	// Bullet Count
	{
		const Entity* entity = getEntity<BulletCountTag>();

		Label* label = entity->GetComponent<Label>();
		label->font = &mUIFont;
		label->text = " ";

		Transform* transform = entity->GetComponent<Transform>();
		transform->position = { .x = float(Constant::Get().GetWidth()) - 140.0f, .y = float(Constant::Get().GetHeight()) - 60.0f };

		Active* active = entity->GetComponent<Active>();
		active->isValue = true;
	}
}

void MainScene::initializeWaveTimer()
{
	const Entity* entity = getEntity<WaveTimerTag>();

	Label* label = entity->GetComponent<Label>();
	label->font = &mUIFont;
	label->text = " ";

	Transform* transform = entity->GetComponent<Transform>();
	transform->position = { .x = Constant::Get().GetHalfWidth() - 150.0f, .y = 0.0f };

	Active* active = entity->GetComponent<Active>();
	active->isValue = true;
}

void MainScene::initializeWaveStage()
{	
	// 현재 웨이브 상태를 초기화한다.
	mGameWaveState =
	{
		.index = 0,
		.groupIndex = 0,
		.waveTimer = WAVES[mGameWaveState.index].elapsedTime,
		.remainingMonsterGroupSpawnTimer = WAVES[mGameWaveState.index].monsterGroupSpawnIntervalTime,
		.labelShowElapsedTimer = 0.0f
	};
	
	const Entity* entity = getEntity<WaveStageTag>();

	Label* label = entity->GetComponent<Label>();
	label->font = &mUIFont;
	label->text = "1 Wave";

	Transform* transform = entity->GetComponent<Transform>();
	transform->position = { .x = Constant::Get().GetHalfWidth() - 80.0f, .y = Constant::Get().GetHalfHeight() - 50.0f };

	Active* active = entity->GetComponent<Active>();
	active->isValue = true;
}

void MainScene::initializePlayer()
{
	constexpr uint32_t MAX_HP = 100;
	constexpr uint32_t MAX_DASH = 5;

	// Information
	{
		const Entity* entity = getEntity<PlayerTag>();

		Transform* transform = entity->GetComponent<Transform>();
		transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };

		Animator* animator = entity->GetComponent<Animator>();
		animator->clipState = &mPlayerClips[uint32_t(Player::eState::Idle)];

		Hp* hp = entity->GetComponent<Hp>();
		hp->max = MAX_HP;
		hp->value = MAX_HP;

		Dash* dash = entity->GetComponent<Dash>();
		dash->maxCount = MAX_DASH;
		dash->count = MAX_DASH;

		Active* active = entity->GetComponent<Active>();
		active->isValue = true;

		BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
		boxCollider->offset = { .x = 0.0f, .y = 32.0f };
		boxCollider->size = { .width = 13.0f, .height = 20.0f };
	}

	// Left Hand
	{
		const Entity* entity = getEntity<LeftHandTag>();

		Transform* transform = entity->GetComponent<Transform>();
		transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };

		Image* image = entity->GetComponent<Image>();
		image->texture = &mPlayerHandTexture;

		Active* active = entity->GetComponent<Active>();
		active->isValue = true;
	}

	// Right Hand
	{
		const Entity* entity = getEntity<RightHandTag>();

		Transform* transform = entity->GetComponent<Transform>();
		transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };

		Image* image = entity->GetComponent<Image>();
		image->texture = &mPlayerHandTexture;

		Active* active = entity->GetComponent<Active>();
		active->isValue = true;
	}

	// Dash Shadow
	{
		const std::vector<Entity*> entities = getEntities<ShadowTag>();
		for (auto& entity : entities)
		{
			Transform* transform = entity->GetComponent<Transform>();
			transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };

			Image* image = entity->GetComponent<Image>();
			image->texture = &mPlayerShadowTexture;

			Active* active = entity->GetComponent<Active>();
			active->isValue = false;

			Color* color = entity->GetComponent<Color>();
			color->a = 0;
		}
	}
}

void MainScene::playerState(const float deltaTime)
{
	static float deadTimer;

	const Entity* entity = getEntity<PlayerTag>();
	const Entity* leftHandEntity = getEntity<LeftHandTag>();
	const Entity* rightHandEntity = getEntity<RightHandTag>();

	Player* player = entity->GetComponent<Player>();
	player->state = (player->length != 0.0f) ? Player::eState::Run : Player::eState::Idle;

	Hp* playerHp = entity->GetComponent<Hp>();
	if (playerHp->value <= 0)
	{
		playerHp->value = 0;

		deadTimer += deltaTime;
		if (deadTimer >= 0.5f)
		{
			player->state = Player::eState::Dead;

			Active* leftHandActive = leftHandEntity->GetComponent<Active>();
			leftHandActive->isValue = false;

			Active* rightHandActive = rightHandEntity->GetComponent<Active>();
			rightHandActive->isValue = false;
		}
	}
}

void MainScene::playerMove(const float deltaTime)
{
	constexpr float MAX_SPEED = 300.0f;
	constexpr float ACC = 62.0f;

	constexpr float KNOCKBACK_FORCE = 0.0f;
	constexpr float KNOCKBACK_COOLTIMER = 0.12f;

	const int32_t moveX = Input::Get().GetKey(SDL_SCANCODE_D) - Input::Get().GetKey(SDL_SCANCODE_A);
	const int32_t moveY = Input::Get().GetKey(SDL_SCANCODE_W) - Input::Get().GetKey(SDL_SCANCODE_S);

	static int32_t prevMoveX;
	static int32_t prevMoveY;

	static Point moveVelocity = {};

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

	// Moving
	Point moveDirection = { .x = 0.0f, .y = 0.0f };
	{
		const Entity* entity = getEntity<PlayerTag>();
		Player* player = entity->GetComponent<Player>();
		player->length = Math::GetVectorLength(moveVelocity);

		moveDirection = Math::NormalizeVector(moveVelocity);
		const float velocity = fmin(Math::GetVectorLength(moveVelocity), MAX_SPEED);
		moveVelocity = moveDirection * velocity;
	}

	// Dash
	{
		constexpr float MAX_DASH_SPEED = 800.0f;
		constexpr float DASH_ACC = 30.0f;

		const Entity* playerEntity = getEntity<PlayerTag>();
		Dash* dash = playerEntity->GetComponent<Dash>();

		if (Input::Get().GetKeyDown(SDL_SCANCODE_SPACE))
		{
			if (dash->count > 0)
			{
				--dash->count;
				dash->isValue = true;
			}
		}

		if (dash->isValue)
		{
			dash->moveSpeed = std::min(dash->moveSpeed + DASH_ACC, MAX_DASH_SPEED);
			const Point velocity = moveDirection * dash->moveSpeed * deltaTime;

			if (dash->moveSpeed >= MAX_DASH_SPEED)
			{
				dash->moveSpeed = 0.0f;
				dash->isValue = false;
				
				dash->isShadow = true;
			}

			Transform* transform = playerEntity->GetComponent<Transform>();
			transform->position += velocity;

			dash->startPosition = transform->position;
			dash->direction = moveDirection;
		}

		if (dash->count < dash->maxCount)
		{
			dash->countTimer += deltaTime;
			if (dash->countTimer >= 2.0f)
			{
				++dash->count;
				dash->countTimer = 0.0f;
			}
		}		
	}

	// Dash Shadow
	{
		constexpr Point OFFSET = { .x = 50.0f, .y = 30.0f };
		constexpr Point INTERVAL_POSITION = { .x = 30.0f, .y = 30.0f };

		// 잔상 색을 업데이트한다.
		std::vector<Entity*> entities = getEntities<ShadowTag>();
		for (const Entity* entity : entities)
		{
			Active* active = entity->GetComponent<Active>();
			if (not active->isValue)
			{
				continue;
			}

			Color* color = entity->GetComponent<Color>();
			const float currentAlpha = static_cast<float>(color->a) - (500.0f * deltaTime);
			color->a = static_cast<uint8_t>(currentAlpha);

			if (currentAlpha <= 0.0f)
			{
				color->a = 0;
				active->isValue = false;
			}
		}

		// 잔상 좌표를 업데이트한다.
		const Entity* playerEntity = getEntity<PlayerTag>();
		Dash* dash = playerEntity->GetComponent<Dash>();
		if (dash->isShadow)
		{
			std::vector<Entity*> shadowEntities = getEntities<ShadowTag>();
			static int32_t index = -1;
			for (Entity* shadowEntity : shadowEntities)
			{
				Active* active = shadowEntity->GetComponent<Active>();
				Color* color = shadowEntity->GetComponent<Color>();

				if (not active->isValue and color->a == 0)
				{
					active->isValue = true;
					color->a = 255;

					Transform* shadowTransform = shadowEntity->GetComponent<Transform>();
					shadowTransform->position = dash->startPosition;
					shadowTransform->position.x -= dash->direction.x * (++index) * 5.0f;
					shadowTransform->position.y += OFFSET.y - dash->direction.y * (++index) * 5.0f;
					shadowTransform->flip = (dash->direction.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

					break;
				}
			}

			dash->shadowTimer += deltaTime;
			if (dash->shadowTimer >= 0.2f)
			{
				index = -1;

				dash->isShadow = false;
				dash->shadowTimer = 0.0f;
			}
		}		
	}

	// Knockback
	{
		const Entity* entity = getEntity<PlayerTag>();
		Knockback* knockback = entity->GetComponent<Knockback>();
		Color* color = entity->GetComponent<Color>();

		if (knockback->isValue)
		{
			color->r = 200;
			color->g = 0;
			color->b = 0;

			moveVelocity += knockback->direction * KNOCKBACK_FORCE;

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
	}

	const Entity* playerEntity = getEntity<PlayerTag>();

	Transform* transform = playerEntity->GetComponent<Transform>();
	clampToTile(transform, { .min = 5.0f, .max = 5.0f }, { .min = -8.0f, .max = 50.0f });
	transform->position += moveVelocity * deltaTime;

	Direction* direction = playerEntity->GetComponent<Direction>();
	direction->value = Math::NormalizeVector(getScreenMousePosition() - transform->position);

	if (direction->value.x > 0.0f)
	{
		transform->flip = SDL_FLIP_NONE;
	}
	else if (direction->value.x < 0.0f)
	{
		transform->flip = SDL_FLIP_HORIZONTAL;
	}

	// Update Left Hand
	{
		constexpr Point OFFSET = { .x = 14.0f, .y = 18.0f };

		const Entity* entity = getEntity<LeftHandTag>();
		Transform* leftHandTransform = entity->GetComponent<Transform>();
		leftHandTransform->position = playerEntity->GetComponent<Transform>()->position;

		leftHandTransform->position.y += OFFSET.y;

		if (playerEntity->GetComponent<Transform>()->flip == SDL_FLIP_NONE)
		{
			leftHandTransform->position.x += OFFSET.x;
		}
		else
		{
			leftHandTransform->position.x -= OFFSET.x;
		}
	}

	// Update Right Hand
	{
		constexpr Point OFFSET = { .x = -10.0f, .y = 15.0f };

		const Entity* rightHandEntity = getEntity<RightHandTag>();
		Transform* rightHandTransform = rightHandEntity->GetComponent<Transform>();
		rightHandTransform->position = playerEntity->GetComponent<Transform>()->position;

		rightHandTransform->position.y += OFFSET.y;

		if (playerEntity->GetComponent<Transform>()->flip == SDL_FLIP_NONE)
		{
			rightHandTransform->position.x += OFFSET.x;
		}
		else
		{
			rightHandTransform->position.x -= OFFSET.x;
		}
	}
}

void MainScene::playerSetClip()
{
	const Entity* entity = getEntity<PlayerTag>();

	Player* player = entity->GetComponent<Player>();
	Animator* animator = entity->GetComponent<Animator>();

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

void MainScene::initializeSwordSkill()
{
	const float SIZE = 2.5f;

	const Entity* entity = getEntity<SwordSkillTag>();

	RangedAttack* rangedAttack = entity->GetComponent<RangedAttack>();
	rangedAttack->distance = 200.0f;

	Transform* transform = entity->GetComponent<Transform>();
	transform->scale = { .width = SIZE, .height = SIZE };

	Animator* animator = entity->GetComponent<Animator>();
	animator->clipState = &mSwordSkillClip;

	BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
	boxCollider->size = { .width = float(mSwordSkillTextures[5].GetWidth()), .height = float(mSwordSkillTextures[5].GetHeight()) };
}

void MainScene::spawnSwordSkill()
{
	const Entity* entity = getEntity<SwordSkillTag>();
	Active* active = entity->GetComponent<Active>();

	if (Effect* effect = entity->GetComponent<Effect>();
		Input::Get().GetMouseButtonDown(SDL_BUTTON_LEFT)
		and not active->isValue
		and not effect->isDisabled)
	{
		Animator* effectAnim = entity->GetComponent<Animator>();
		effectAnim->frameIndex = 0;
		effectAnim->elapsedTime = 0.0f;

		active->isValue = true;

		const Entity* playerEntity = getEntity<PlayerTag>();
		const Transform* playerTransform = playerEntity->GetComponent<Transform>();

		RangedAttack* rangedAttack = entity->GetComponent<RangedAttack>();
		rangedAttack->startPosition = playerTransform->position;

		Transform* effectTransform = entity->GetComponent<Transform>();
		effectTransform->position = playerTransform->position;

		const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;
		Direction* effectDirection = entity->GetComponent<Direction>();
		effectDirection->value = Math::NormalizeVector(mouseToPlayer);

		float degree = std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);
		degree -= 90.0f;
		effectTransform->angle = -degree;

		effectTransform->flip = (effectDirection->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	}
}

void MainScene::updateSwordSkill(const float deltaTime)
{
	constexpr float SPEED = 900.0f;
	constexpr float SWING_COOLTIME = 0.7f;

	const Entity* entity = getEntity<SwordSkillTag>();
	if (const Active* active = entity->GetComponent<Active>();
		not active->isValue)
	{
		return;
	}

	const Direction* direction = entity->GetComponent<Direction>();
	const Point velocity = direction->value * SPEED;

	Transform* transform = entity->GetComponent<Transform>();
	transform->position = transform->position + velocity * deltaTime;
}

void MainScene::updateSwordSkillStates(const float deltaTime)
{
	constexpr float SWING_COOLTIME = 0.7f;

	const Entity* entity = getEntity<SwordSkillTag>();
	Effect* effect = entity->GetComponent<Effect>();
	Active* active = entity->GetComponent<Active>();

	if (active->isValue)
	{
		Transform* transform = entity->GetComponent<Transform>();
		if (const RangedAttack* rangedAttack = entity->GetComponent<RangedAttack>();
			Math::GetVectorLength(transform->position - rangedAttack->startPosition) >= rangedAttack->distance)
		{
			Animator* effectAnim = entity->GetComponent<Animator>();
			effectAnim->frameIndex = 0;
			effectAnim->elapsedTime = 0.0f;

			active->isValue = false;
			effect->isDisabled = true;

			effect->coolTime = SWING_COOLTIME;
		}
	}
	else
	{
		effect->coolTime -= deltaTime;
		if (effect->coolTime <= 0.0f)
		{
			effect->isDisabled = false;
			effect->coolTime = SWING_COOLTIME;
		}
	}
}

void MainScene::initializeBullets()
{
	mBulletState =
	{
		.maxCount = 30,
		.count = 30,
		.fireTimer = 0.0f,
		.reloadTimer = 0.0f
	};

	const std::vector<Entity*> entities = getEntities<BulletTag>();
	for (const Entity* entity : entities)
	{
		RangedAttack* rangedAttack = entity->GetComponent<RangedAttack>();
		rangedAttack->distance = 300.0f;

		Transform* transform = entity->GetComponent<Transform>();
		transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };

		Animator* animator = entity->GetComponent<Animator>();
		animator->clipState = &mBulletClip;

		BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
		boxCollider->size = { .width = float(mBulletTextures[5].GetWidth()), .height = float(mBulletTextures[5].GetHeight()) };
	}
}

void MainScene::spawnBullets(const float deltaTime)
{
	constexpr float FIRE_TIME = 0.8f;

	const Entity* gunEntity = getEntity<GunTag>();
	const Transform* gunTransform = gunEntity->GetComponent<Transform>();

	if (mBulletState.count <= 0)
	{
		return;
	}

	const std::vector<Entity*> entities = getEntities<BulletTag>();

	mBulletState.fireTimer += deltaTime;
	for (const Entity* entity : entities)
	{
		Active* active = entity->GetComponent<Active>();
		if (active->isValue)
		{
			continue;
		}

		RangedAttack* rangedAttack = entity->GetComponent<RangedAttack>();
		if (rangedAttack->isFiring)
		{
			continue;
		}

		if (mBulletState.fireTimer >= FIRE_TIME)
		{
			active->isValue = true;
			rangedAttack->isFiring = true;

			Animator* anim = entity->GetComponent<Animator>();
			anim->frameIndex = 0;
			anim->elapsedTime = 0.0f;

			rangedAttack->startPosition = gunTransform->position;

			Transform* transform = entity->GetComponent<Transform>();
			transform->position = gunTransform->position;

			const Point difference = getScreenMousePosition() - gunTransform->position;
			Direction* direction = entity->GetComponent<Direction>();
			direction->value = Math::NormalizeVector(difference);

			--mBulletState.count;

			const Entity* bulletCountEntity = getEntity<BulletCountTag>();
			Label* label = bulletCountEntity->GetComponent<Label>();
			const std::string strLabel = (mBulletState.count < 10) ? "0" + std::to_string(mBulletState.count) : std::to_string(mBulletState.count);
			label->text = strLabel + "/" + std::to_string(mBulletState.maxCount);

			mBulletState.fireTimer = 0.0f;
			break;
		}
	}
}

void MainScene::updateBullets(const float deltaTime)
{
	constexpr float SPEED = 1300.0f;

	const std::vector<Entity*> entities = getEntities<BulletTag>();
	for (const Entity* entity : entities)
	{
		if (const Active* active = entity->GetComponent<Active>();
			not active->isValue)
		{
			continue;
		}

		const Direction* direction = entity->GetComponent<Direction>();
		const Point velocity = direction->value * SPEED;
		Transform* transform = entity->GetComponent<Transform>();
		transform->position += velocity * deltaTime;
	}
}

void MainScene::updateBulletStates(const float deltaTime)
{
	constexpr float RELOAD_TIME = 1.2f;

	if (mBulletState.count <= 0)
	{
		mBulletState.reloadTimer += deltaTime;
		if (mBulletState.reloadTimer >= RELOAD_TIME)
		{
			mBulletState.count = mBulletState.maxCount;
			mBulletState.reloadTimer = 0.0f;
		}
	}

	const std::vector<Entity*> entities = getEntities<BulletTag>();
	for (const Entity* entity : entities)
	{
		if (const Active* active = entity->GetComponent<Active>();
			not active->isValue)
		{
			continue;
		}

		RangedAttack* rangedAttack = entity->GetComponent<RangedAttack>();
		if (not rangedAttack->isFiring)
		{
			continue;
		}

		Transform* transform = entity->GetComponent<Transform>();
		if (Math::GetVectorLength(transform->position - rangedAttack->startPosition) >= rangedAttack->distance)
		{
			Active* active = entity->GetComponent<Active>();
			active->isValue = false;
			rangedAttack->isFiring = false;
		}
	}
}

void MainScene::spawnMonsterGroup(const MonsterGroup& group)
{
	constexpr uint32_t MONSTER_COUNT = 20;
	constexpr Point TILE_OFFSET = { .x = 50.0f, .y = 50.0f };
	
	const Rect tileRect =
	{
		.left = mTiles[0][0].GetComponent<Transform>()->position.x,
		.top = mTiles[0][0].GetComponent<Transform>()->position.y,
		.right = mTiles[mTileMaxCount - 1][mTileMaxCount - 1].GetComponent<Transform>()->position.x,
		.bottom = mTiles[mTileMaxCount - 1][mTileMaxCount - 1].GetComponent<Transform>()->position.y
	};

	const Point randomTilePosition =
	{
		.x = getRandom(tileRect.left + TILE_OFFSET.x, tileRect.right - TILE_OFFSET.x),
		.y = getRandom(tileRect.bottom + TILE_OFFSET.y, tileRect.top - TILE_OFFSET.y)
	};
	
	for (uint32_t i = 0; i < group.count; ++i)
	{
		const float xInGroup = getRandom(group.rangeX.min, group.rangeX.max);
		const float yInGroup = getRandom(group.rangeY.min, group.rangeY.max);

		for (__noop; mMonsterIndex < MONSTER_COUNT; ++mMonsterIndex)
		{
			const float monsterX = randomTilePosition.x + xInGroup;
			const float monsterY = randomTilePosition.y + yInGroup;
			
			spawnMonster
			(
				SpawnMonsterDesc
				{
					.index = mMonsterIndex,
					.type = group.type,
					.x = monsterX,
					.y = monsterY
				}
			);

			break;
		}
		
		++mMonsterIndex;
	}
}

void MainScene::spawnMonster(const SpawnMonsterDesc& desc)
{
	constexpr float SIZE = 0.7f;

	constexpr Point BACK_OFFSET = { .x = 20.0f, .y = 70.0f };
	constexpr Point LEFT_OFFSET = { .x = 20.0f, .y = 0.0f };
	constexpr Point RIGHT_OFFSET = { .x = 200.0f, .y = 120.0f };

	const uint32_t index = desc.index;
	const eMonsterType type = desc.type;
	const float x = desc.x;
	const float y = desc.y;

	if (type == eMonsterType::Boss)
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(BossBackTag());
		entity->AddComponent(Transform());
		entity->AddComponent(Image());
		entity->AddComponent(Animator());
		entity->AddComponent(Direction());
		entity->AddComponent(Active());

		Transform* transform = entity->GetComponent<Transform>();
		transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };

		Animator* animator = entity->GetComponent<Animator>();
		animator->clipState = &mBossBackClip;
	}

	// 몬스터 컴포넌트를 추가합니다.
	Entity* entity = GetEntityWorld()->AddEntity(new Entity());
	{
		entity->AddComponent(Monster());
		entity->AddComponent(AttackPattern());
		entity->AddComponent(Transform());
		entity->AddComponent(Image());
		entity->AddComponent(Animator());
		entity->AddComponent(Direction());
		entity->AddComponent(DamageTimer());
		entity->AddComponent(Hp());
		entity->AddComponent(Damage());
		entity->AddComponent(Active());
		entity->AddComponent(Color());
		entity->AddComponent(BoxCollider());
		entity->AddComponent(ColliderState());
		entity->AddComponent(DebugActive());
		entity->AddComponent(DebugColor());
		entity->AddComponent(Knockback());

		CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Monster));
		collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Monster));
		entity->AddComponent(collider);
	}
	
	Monster* monster = entity->GetComponent<Monster>();
	monster->type = type;
	monster->state = Monster::eState::Spawn;

	Transform* transform = entity->GetComponent<Transform>();
	transform->position = { .x = x, .y = y };
	transform->scale = { .width = SIZE, .height = SIZE };

	DamageTimer* damageTimer = entity->GetComponent<DamageTimer>();
	damageTimer->deadTimer = 0.0f;

	Hp* hp = entity->GetComponent<Hp>();
	hp->hpBarIndex = index;

	BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
	boxCollider->size = {.width = SIZE, .height = SIZE };
	boxCollider->offset = { .x = 0.0f, .y = 0.0f };

	ColliderState* colliderState = entity->GetComponent<ColliderState>();
	
	Damage* damage = entity->GetComponent<Damage>();
	
	AttackPattern* pattern = entity->GetComponent<AttackPattern>();
	
	// 체력바 컴포넌트를 추가합니다.
	Entity* hpBarEntity = GetEntityWorld()->AddEntity(new Entity());
	hpBarEntity->AddComponent(MonsterHpBarTag());
	hpBarEntity->AddComponent(Transform());
	hpBarEntity->AddComponent(Offset());
	hpBarEntity->AddComponent(Image());
	hpBarEntity->AddComponent(Active());
	
	Offset* hpBarOffset = hpBarEntity->GetComponent<Offset>();
	
	switch (monster->type)
	{
	case eMonsterType::BigWhite:
		entity->AddComponent(MonsterTag());
		hp->max = 2;
		damage->value = 1;
		pattern->type = AttackPattern::eType::None;
		pattern->timer = 0.0f;
		monster->attackType = eAttackType::Melee;
		monster->speed = 35.0f;
		monster->attackDistance = 90.0f;
		monster->clips = mBigWhiteSkelClips.data();
		colliderState->runSize = { .width = 10.0f, .height = 30.0f };
		colliderState->runOffset = { .x = 0.0f, .y = 45.0f };
		colliderState->attackSize = { .width = 30.0f, .height = 60.0f };
		colliderState->attackOffset = { .x = 70.0f, .y = 45.0f };
		colliderState->attackAnimIndex = 5;
		hpBarOffset->value = { .x = 0.0f, .y = -20.0f };
		break;

	case eMonsterType::Archer:
		hp->max = 3;
		damage->value = 2;
		pattern->type = AttackPattern::eType::None;
		pattern->timer = 0.0f;
		monster->attackType = eAttackType::Range;
		monster->speed = 50.0f;
		monster->attackDistance = 300.0f;
		monster->clips = mArcherClips.data();
		colliderState->runSize = { .width = float(mArcherRunTextures[1].GetWidth()), .height = float(mArcherRunTextures[1].GetHeight()) };
		colliderState->runOffset = { .x = 0.0f, .y = 0.0f };
		colliderState->attackSize = { .width = float(mArcherAttackTextures[7].GetWidth()), .height = float(mArcherAttackTextures[7].GetHeight()) };
		colliderState->attackOffset = { .x = 0.0f, .y = 0.0f };
		colliderState->attackAnimIndex = 0;
		hpBarOffset->value = { .x = 10.0f, .y = -55.0f };
		break;

	case eMonsterType::SkelDog:
		entity->AddComponent(MonsterTag());
		hp->max = 1;
		damage->value = 1;
		pattern->type = AttackPattern::eType::Rush;
		pattern->timer = 0.0f;
		monster->attackType = eAttackType::Melee;
		monster->speed = 60.0f;
		monster->attackDistance = 200.0f;
		monster->clips = mSkelDogClips.data();
		colliderState->runSize = { .width = 15.0f, .height = 15.0f };
		colliderState->runOffset = { .x = 0.0f, .y = -5.0f };
		colliderState->attackSize = { .width = 15.0f, .height = 15.0f };
		colliderState->attackOffset = { .x = 0.0f, .y = -5.0f };
		colliderState->attackAnimIndex = 5;
		hpBarOffset->value = { .x = 0.0f, .y = -40.0f };
		break;

	case eMonsterType::Boss:
	{
		entity->AddComponent(BossTag());
		
		const Entity* cameraEntity = getEntity<Camera>();
		const Transform* cameraTransform = cameraEntity->GetComponent<Transform>();
		transform->position = { .x = cameraTransform->position.x - 120.0f, .y = cameraTransform->position.y + Constant::Get().GetHalfHeight() - 200.0f };

		hp->max = 100;
		damage->value = 10;
		pattern->type = AttackPattern::eType::CycloneFan;
		pattern->timer = 0.0f;
		monster->attackType = eAttackType::Range;
		monster->speed = 300.0f;
		monster->attackDistance = 1.0f;
		monster->clips = mBossClips.data();
		colliderState->runSize = { .width = 55.0f, .height = 75.0f };
		colliderState->runOffset = { .x = 18.0f, .y = -15.0f };
		colliderState->attackSize = { .width = 1.0f, .height = 1.0f };
		colliderState->attackOffset = { .x = 0.0f, .y = 0.0f };
		colliderState->attackAnimIndex = 5;

		const Entity* backEntity = getEntity<BossBackTag>();
		Transform* backTransform = backEntity->GetComponent<Transform>();
		backTransform->position = { .x = transform->position.x + BACK_OFFSET.x, .y = transform->position.y - BACK_OFFSET.y };

		// Left Hand
		{
			Entity* entity = GetEntityWorld()->AddEntity(new Entity());
			entity->AddComponent(BossLeftHandTag());
			entity->AddComponent(Transform());
			entity->AddComponent(Image());
			entity->AddComponent(Animator());
			entity->AddComponent(Direction());
			entity->AddComponent(Active());

			Transform* transform = entity->GetComponent<Transform>();
			transform->position.x = LEFT_OFFSET.x - Constant::Get().GetHalfWidth();
			transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };

			Animator* animator = entity->GetComponent<Animator>();
			animator->clipState = &mBossLeftHandClip;
		}

		// Right Hand
		{
			Entity* entity = GetEntityWorld()->AddEntity(new Entity());
			entity->AddComponent(BossRightHandTag());
			entity->AddComponent(Transform());
			entity->AddComponent(Image());
			entity->AddComponent(Animator());
			entity->AddComponent(Direction());
			entity->AddComponent(Active());

			Transform* transform = entity->GetComponent<Transform>();
			transform->position.x = Constant::Get().GetHalfWidth() - RIGHT_OFFSET.x;
			transform->position.y = Constant::Get().GetHalfHeight() - RIGHT_OFFSET.y;
			transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
			transform->flip = SDL_FLIP_HORIZONTAL;

			Animator* animator = entity->GetComponent<Animator>();
			animator->clipState = &mBossRightHandClip;
		}
	}
		break;

	default:
		assert(false and "지원하지 않는 몬스터 유형입니다.");
		break;
	}

	Animator* anim = entity->GetComponent<Animator>();
	anim->frameIndex = 0;
	anim->elapsedTime = 0.0f;

	hp->value = hp->max;

	Active* active = entity->GetComponent<Active>();
	active->isValue = true;
	
	// 몬스터 체력바
	{
		Image* image = hpBarEntity->GetComponent<Image>();
		image->texture = &mRedRectTexture;

		Active* hpBarActive = hpBarEntity->GetComponent<Active>();
		hpBarActive->isValue = false;

		Transform* hpBartransform = hpBarEntity->GetComponent<Transform>();
		const float currentWidth = (static_cast<float>(hp->value) / hp->max) * 0.8f;
		hpBartransform->scale = { .width = currentWidth, .height = 0.2f };
	}
}

void MainScene::updateMonsterStates(const float deltaTime)
{
	const std::vector<Entity*> entities = getEntities<MonsterTag>();
	for (Entity* entity : entities)
	{
		if (not entity->HasComponent<MonsterTag>())
		{
			continue;
		}

		if (not entity->GetComponent<Active>()->isValue)
		{
			continue;
		}

		const ColliderState* colliderState = entity->GetComponent<ColliderState>();
		const Direction* direction = entity->GetComponent<Direction>();

		BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();

		Monster* monster = entity->GetComponent<Monster>();
		AttackPattern* pattern = entity->GetComponent<AttackPattern>();

		switch (monster->state)
		{
		case Monster::eState::Spawn:
		{
			monster->spawnBlinkTimer += deltaTime;
			if (monster->spawnBlinkTimer >= 0.5f)
			{
				Transform* monsterTransform = entity->GetComponent<Transform>();
				monsterTransform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };

				monster->state = Monster::eState::Run;

				Active* active = entity->GetComponent<Active>();
				active->isValue = true;

				monster->spawnBlinkTimer = 0.0f;
			}

			break;
		}

		case Monster::eState::Idle:
		{
			constexpr float RUSH_WAITINGTIME = 2.0f;

			boxCollider->offset = colliderState->runOffset;
			boxCollider->size = colliderState->runSize;

			if (monster->type == eMonsterType::SkelDog)
			{
				float gauge = pattern->timer / RUSH_WAITINGTIME;
				gauge = std::clamp(gauge, 0.0f, 1.0f);

				Color* color = entity->GetComponent<Color>();
				color->r = 255 - static_cast<uint8_t>(gauge * 255.0f);
				color->g = 255 - static_cast<uint8_t>(gauge * 255.0f);

				AttackPattern* pattern = entity->GetComponent<AttackPattern>();
				pattern->timer += deltaTime;
				if (pattern->timer >= RUSH_WAITINGTIME)
				{
					color->r = 255;
					color->g = 255;

					const Entity* playerEntity = getEntity<PlayerTag>();
					const Transform* playerTransform = playerEntity->GetComponent<Transform>();
					
					const Transform* transfrom = entity->GetComponent<Transform>();
					const Point difference = playerTransform->position - transfrom->position;
					
					Direction* direction = entity->GetComponent<Direction>();
					direction->value = Math::NormalizeVector(difference);

					monster->state = Monster::eState::Rush;
					pattern->timer = 0.0f;
				}
			}
		
			break;
		}

		case Monster::eState::Attack:
		{
			if (pattern->type == AttackPattern::eType::Rush)
			{
				monster->state = Monster::eState::Idle;
				break;
			}

			const Clip& attackClip = monster->clips[uint32_t(Monster::eState::Attack)];
			const Animator& anim = *entity->GetComponent<Animator>();
			if (anim.clipState == &attackClip
				and anim.frameIndex >= attackClip.GetLastFrameIndex() - 1)
			{
				if (monster->length > monster->attackDistance)
				{
					monster->state = Monster::eState::Run;
				}
			}

			break;
		}

		case Monster::eState::Run:
		{
			boxCollider->size = colliderState->runSize;
			boxCollider->offset = colliderState->runOffset;

			if (monster->length <= monster->attackDistance)
			{
				monster->state = Monster::eState::Attack;
			}

			break;
		}

		case Monster::eState::Rush:
		{
			constexpr float SPEED = 600.0f;
			constexpr float RUSH_DISTANCE = 500.0f;

			boxCollider->size = colliderState->runSize;
			boxCollider->offset = colliderState->runOffset;

			float movingDistance = SPEED * deltaTime;
			pattern->distanceMoved += movingDistance;

			const Direction* direction = entity->GetComponent<Direction>();
			const Point velocity = direction->value * SPEED;

			Transform* transform = entity->GetComponent<Transform>();
			if (pattern->distanceMoved >= RUSH_DISTANCE)
			{
				float overDistance = pattern->distanceMoved - RUSH_DISTANCE;
				movingDistance -= overDistance;
				transform->position += direction->value * movingDistance;

				pattern->distanceMoved = 0.0f;
				monster->state = Monster::eState::Run;
			}
			else
			{
				clampToTile(transform, { .min = 5.0f, .max = 5.0f }, { .min = -8.0f, .max = 50.0f });
				transform->position += velocity * deltaTime;
			}

			break;
		}

		case Monster::eState::Dead:
		{
			__noop;
			break;
		}
		default:
			assert(false and "지원하지 않는 애니메이션입니다.");
			break;
		}
	}

	for (Entity* entity : entities)
	{
		if (not entity->HasComponent<MonsterTag>())
		{
			continue;
		}

		Hp* hp = entity->GetComponent<Hp>();
		if (hp->value > 0)
		{
			continue;
		}

		hp->value = 0;

		Active* active = entity->GetComponent<Active>();
		active->isValue = false;

		Monster* monster = entity->GetComponent<Monster>();
		monster->state = Monster::eState::Dead;

		if (const Entity* hpBarEntity = getEntities<MonsterHpBarTag>()[hp->hpBarIndex];
			hpBarEntity->HasComponent<Active>())
		{
			if (Active* hpBarActive = hpBarEntity->GetComponent<Active>())
			{
				hpBarActive->isValue = false;
			}
		}
	}
}

void MainScene::updateHpMonsters(const float deltaTime)
{
	constexpr float DAMAGE_TIME = 0.12f;

	const std::vector<Entity*> entities = getEntities<MonsterTag>();
	for (Entity* entity : entities)
	{
		if (not entity->HasComponent<MonsterTag>())
		{
			continue;
		}

		if (Knockback* knockback = entity->GetComponent<Knockback>();
			knockback->isValue)
		{
			Color* color = entity->GetComponent<Color>();
			color->r = 200;
			color->g = 0;
			color->b = 0;

			knockback->coolTimer += deltaTime;
			if (knockback->coolTimer >= DAMAGE_TIME)
			{
				Monster* monster = entity->GetComponent<Monster>();
				monster->state = Monster::eState::Run;

				Color* color = entity->GetComponent<Color>();
				color->r = 255;
				color->g = 255;
				color->b = 255;

				knockback->isValue = false;
				knockback->coolTimer = 0.0f;
			}
		}
	}
}

void MainScene::monsterMove(const float deltaTime)
{
	const std::vector<Entity*> entities = getEntities<MonsterTag>();
	for (Entity* entity : entities)
	{
		if (not entity->HasComponent<MonsterTag>())
		{
			continue;
		}

		Monster* monster = entity->GetComponent<Monster>();
		if (monster->type == eMonsterType::None)
		{
			continue;
		}
		
		Transform* monsterTransform = entity->GetComponent<Transform>();

		const Entity* playerEntity = getEntity<PlayerTag>();
		const Transform* playerTransform = playerEntity->GetComponent<Transform>();

		const Point difference = playerTransform->position - monsterTransform->position;
		monster->length = Math::GetVectorLength(difference);

		if (monster->state != Monster::eState::Run)
		{
			continue;
		}

		Direction* direction = entity->GetComponent<Direction>();
		direction->value = Math::NormalizeVector(difference);

		const Point velocity = direction->value * monster->speed;

		// TODO: 플레이어 Knockback
		//Knockback* playerKnockback = mPlayer.GetComponent<Knockback>();
		//playerKnockback->direction = direction->value;

		clampToTile(monsterTransform, { .min = 5.0f, .max = 5.0f }, { .min = -8.0f, .max = 50.0f });
		monsterTransform->position += velocity * deltaTime;
		monsterTransform->flip = (direction->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	}
}

void MainScene::monsterHpBarMove()
{
	const std::vector<Entity*> entities = getEntities<MonsterTag>();
	for (const Entity* monsterEntity : entities)
	{
		if (const Monster* monster = monsterEntity->GetComponent<Monster>(); 
			monster->state == Monster::eState::Spawn
			or monster->state == Monster::eState::Dead)
		{
			continue;
		}

		const Hp* hp = monsterEntity->GetComponent<Hp>();
		Entity* hpBarEntity = getEntities<MonsterHpBarTag>()[hp->hpBarIndex];

		Active* hpBarActive = hpBarEntity->GetComponent<Active>();
		hpBarActive->isValue = true;

		setDirectionOffset(hpBarEntity, *monsterEntity);
	}
}

void MainScene::setDirectionOffset(Entity* setEntity, const Entity& entity)
{
	const Offset* offset = setEntity->GetComponent<Offset>();

	Transform* setEntityTransform = setEntity->GetComponent<Transform>();
	const Transform* entityTransform = entity.GetComponent<Transform>();

	setEntityTransform->position.x = (entityTransform->flip == SDL_FLIP_NONE)
		? entityTransform->position.x - offset->value.x
		: entityTransform->position.x + offset->value.x;

	setEntityTransform->position.y = entityTransform->position.y + offset->value.y;
}

void MainScene::monsterSetClip()
{
	const std::vector<Entity*> entities = getEntities<MonsterTag>();
	for (Entity* entity : entities)
	{
		if (not entity->HasComponent<MonsterTag>())
		{
			continue;
		}

		Animator* animator = entity->GetComponent<Animator>();
		Monster* monster = entity->GetComponent<Monster>();

		if (animator == nullptr)
		{
			continue;
		}

		switch (monster->state)
		{
		case Monster::eState::Spawn:
			animator->SetClip(&monster->clips[uint32_t(Monster::eState::Spawn)]);
			break;

		case Monster::eState::Idle:
			animator->SetClip(&monster->clips[uint32_t(Monster::eState::Idle)]);
			break;

		case Monster::eState::Run:
			animator->SetClip(&monster->clips[uint32_t(Monster::eState::Run)]);
			break;

		case Monster::eState::Attack:
			animator->SetClip(&monster->clips[uint32_t(Monster::eState::Attack)]);
			break;

		case Monster::eState::Rush:
			__noop;
			break;

		case Monster::eState::Dead:
			__noop;
			break;

		default:
			assert(false and "지원하지 않는 애니메이션입니다.");
			break;
		}
	}
}

void MainScene::spawnCycloneFan(const float deltaTime)
{
	constexpr float FIRE_TIME = 0.12f;

	for (uint32_t i = 0; i < mCycloneFanState.maxCount; ++i)
	{
		Entity* newEntity = GetEntityWorld()->AddEntity(new Entity());
		if (newEntity->HasComponent<CycloneFanTag>())
		{
			continue;
		}

		newEntity->AddComponent(CycloneFanTag());
		newEntity->AddComponent(RangedAttack());
		newEntity->AddComponent(Direction());
		newEntity->AddComponent(Transform());
		newEntity->AddComponent(Image());
		newEntity->AddComponent(Animator());
		newEntity->AddComponent(Active());
		newEntity->AddComponent(BoxCollider());
		newEntity->AddComponent(DebugActive());
		newEntity->AddComponent(DebugColor());

		RangedAttack* rangedAttack = newEntity->GetComponent<RangedAttack>();
		rangedAttack->distance = 600.0f;

		Transform* transform = newEntity->GetComponent<Transform>();
		transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };

		Animator* animator = newEntity->GetComponent<Animator>();
		animator->clipState = &mCycloneFanClip;

		CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::CycloneFan));
		collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Player));
		newEntity->AddComponent(collider);

		BoxCollider* boxCollider = newEntity->GetComponent<BoxCollider>();
		boxCollider->size = { .width = float(mCycloneFanTextures[0].GetWidth()), .height = float(mCycloneFanTextures[0].GetHeight()) };
	}

	if (mCycloneFanState.count <= 0)
	{
		return;
	}

	mCycloneFanState.fireTimer += deltaTime;
	if (mCycloneFanState.fireTimer >= FIRE_TIME)
	{
		for (auto entities = getEntities<CycloneFanTag>();
			Entity* entity : entities)
		{
			if (entity->IsRemove())
			{
				continue;
			}

			if (const RangedAttack* rangedAttack = entity->GetComponent<RangedAttack>();
				rangedAttack->isFiring)
			{
				continue;
			}

			--mCycloneFanState.count;

			Active* active = entity->GetComponent<Active>();
			active->isValue = true;

			const Entity* bossEntity = getEntity<BossTag>();
			const Point bossPosition = bossEntity->GetComponent<Transform>()->position;

			RangedAttack* rangedAttack = entity->GetComponent<RangedAttack>();
			rangedAttack->isFiring = true;
			rangedAttack->startPosition = bossPosition;

			Transform* transform = entity->GetComponent<Transform>();
			transform->position = bossPosition;

			Direction* direction = entity->GetComponent<Direction>();
			direction->value.y = -1.0f;

			mCycloneFanState.fireTimer = 0.0f;
			
			break;
		}
	}
}

void MainScene::updateBossStates(const float deltaTime)
{
	constexpr float BOSS_ATTACKTIME = 2.0f;
	
	const Entity* entity = getEntity<BossTag>();
	if (entity == nullptr)
	{
		return;
	}

	const ColliderState* colliderState = entity->GetComponent<ColliderState>();
	const Direction* direction = entity->GetComponent<Direction>();

	BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
	AttackPattern* pattern = entity->GetComponent<AttackPattern>();

	Monster* monster = entity->GetComponent<Monster>();
	switch (monster->state)
	{
	case Monster::eState::Spawn:
	{
		monster->spawnBlinkTimer += deltaTime;
		if (monster->spawnBlinkTimer >= 0.5f)
		{
			Transform* monsterTransform = entity->GetComponent<Transform>();
			monsterTransform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
			
			monster->state = Monster::eState::Idle;

			const Entity* leftHandEntity = getEntity<BossLeftHandTag>();
			Active* leftActive = leftHandEntity->GetComponent<Active>();
			leftActive->isValue = true;

			const Entity* rightHandEntity = getEntity<BossRightHandTag>();
			Active* rightActive = rightHandEntity->GetComponent<Active>();
			rightActive->isValue = true;

			const Entity* backEntity = getEntity<BossBackTag>();
			Active* backActive = backEntity->GetComponent<Active>();
			backActive->isValue = true;
			
			Active* active = entity->GetComponent<Active>();
			active->isValue = true;

			monster->spawnBlinkTimer = 0.0f;
		}

		break;
	}

	case Monster::eState::Idle:
	{
		boxCollider->offset = colliderState->runOffset;
		boxCollider->size = colliderState->runSize;

		AttackPattern* pattern = entity->GetComponent<AttackPattern>();
		pattern->timer += deltaTime;
		if (pattern->timer >= BOSS_ATTACKTIME)
		{
			monster->state = Monster::eState::Attack;
			pattern->timer = 0.0f;
		}

		break;
	}

	case Monster::eState::Attack:
	{
		if (pattern->type == AttackPattern::eType::CycloneFan)
		{
			spawnCycloneFan(deltaTime);
			updateCycloneFan(deltaTime);
			break;
		}

		const Clip& attackClip = monster->clips[uint32_t(Monster::eState::Attack)];
		const Animator& anim = *entity->GetComponent<Animator>();
		if (anim.clipState == &attackClip
			and anim.frameIndex >= attackClip.GetLastFrameIndex() - 1)
		{
			// TODO: 입 계속 열어두게 하기
		}

		break;
	}

	case Monster::eState::Run:
	{
		boxCollider->size = colliderState->runSize;
		boxCollider->offset = colliderState->runOffset;

		break;
	}

	case Monster::eState::Rush:
		__noop;
		break;

	case Monster::eState::Dead:
		__noop;
		break;

	default:
		assert(false and "지원하지 않는 애니메이션입니다.");
		break;
	}
	
	Hp* hp = entity->GetComponent<Hp>();
	if (hp->value > 0)
	{
		return;
	}

	hp->value = 0;

	Active* active = entity->GetComponent<Active>();
	active->isValue = false;

	monster->state = Monster::eState::Dead;
}

void MainScene::updateCycloneFan(const float deltaTime)
{
	constexpr float SPEED = 400.0f;

	if (mCycloneFanState.count <= 0)
	{
		mCycloneFanState.reloadTimer += deltaTime;
		if (mCycloneFanState.reloadTimer >= 3.0f)
		{
			mCycloneFanState.count = mCycloneFanState.maxCount;
			mCycloneFanState.reloadTimer = 0.0f;
		}
	}

	for (auto entities = getEntities<CycloneFanTag>();
		Entity* entity : entities)
	{
		if (entity->IsRemove())
		{
			continue;
		}

		const Direction* direction = entity->GetComponent<Direction>();
		const Point velocity = direction->value * SPEED;

		Transform* transform = entity->GetComponent<Transform>();
		transform->position += velocity * deltaTime;
	}

	for (auto entities = getEntities<CycloneFanTag>();
		Entity * entity : entities)
	{
		if (entity->IsRemove())
		{
			continue;
		}

		Transform* transform = entity->GetComponent<Transform>();
		if (transform->position.x >= getCameraRect().left
			and transform->position.x <= getCameraRect().right
			and transform->position.y >= getCameraRect().bottom
			and transform->position.y <= getCameraRect().top)
		{
			continue;
		}
		
		entity->SetRemove();
		entity->RemovedComponent();
	}
}

void MainScene::BossSetClip()
{	
	const Entity* entity = getEntity<BossTag>();
	if (entity == nullptr)
	{
		return;
	}

	Animator* animator = entity->GetComponent<Animator>();
	Monster* monster = entity->GetComponent<Monster>();

	switch (monster->state)
	{
	case Monster::eState::Spawn:
		animator->SetClip(&monster->clips[uint32_t(Monster::eState::Spawn)]);
		break;

	case Monster::eState::Idle:
		animator->SetClip(&monster->clips[uint32_t(Monster::eState::Idle)]);
		break;

	case Monster::eState::Run:
		animator->SetClip(&monster->clips[uint32_t(Monster::eState::Run)]);
		break;

	case Monster::eState::Attack:
		animator->SetClip(&monster->clips[uint32_t(Monster::eState::Attack)]);
		break;

	case Monster::eState::Rush:
		__noop;
		break;

	case Monster::eState::Dead:
		__noop;
		break;

	default:
		assert(false and "지원하지 않는 애니메이션입니다.");
		break;
	}
}

void MainScene::spawnAttackCollider()
{
	for (const std::vector<Entity*> entities = getEntities<MonsterTag>(); 
		Entity* monsterEntity : entities)
	{
		if (not monsterEntity->HasComponent<MonsterTag>())
		{
			continue;
		}

		const Monster* monster = monsterEntity->GetComponent<Monster>();
		if (monster->attackType != eAttackType::Melee)
		{
			continue;
		}

		if (monster->state != Monster::eState::Attack)
		{
			continue;
		}

		const Animator* anim = monsterEntity->GetComponent<Animator>();
		const ColliderState* state = monsterEntity->GetComponent<ColliderState>();

		if (anim->frameIndex == state->attackAnimIndex)
		{
			for (const std::vector<Entity*> entities = getEntities<MonsterAttackColliderTag>();
				Entity* attackEntity : entities)
			{
				if (MonsterAttackCollider* attackCollider = attackEntity->GetComponent<MonsterAttackCollider>(); 
					attackCollider->ownerEntity == monsterEntity)
				{
					continue;
				}
			}

			Entity* attackAntity = GetEntityWorld()->AddEntity(new Entity());

			MonsterAttackCollider attack{};
			attack.ownerEntity = monsterEntity;
			attackAntity->AddComponent(attack);

			attackAntity->AddComponent(MonsterAttackColliderTag());
			attackAntity->AddComponent(Damage());
			attackAntity->AddComponent(Transform());
			attackAntity->AddComponent(Direction());
			attackAntity->AddComponent(Active());
			attackAntity->AddComponent(BoxCollider());
			attackAntity->AddComponent(DebugColor());

			CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Monster));
			collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Player));
			attackAntity->AddComponent(collider);

			DebugActive debugActive{};
			debugActive.isValue = mIsDebugActive;
			attackAntity->AddComponent(debugActive);
		}
	}
}

void MainScene::updateAttackCollision()
{
	for (const std::vector<Entity*> entities = getEntities<MonsterAttackColliderTag>();
		Entity* attackEntity : entities)
	{
		if (not attackEntity->HasComponent<MonsterAttackCollider>())
		{
			continue;
		}

		const MonsterAttackCollider* attackCollider = attackEntity->GetComponent<MonsterAttackCollider>();
		const Entity* monsterEntity = attackCollider->ownerEntity;
		if (monsterEntity == nullptr)
		{
			continue;
		}

		const Monster* monster = monsterEntity->GetComponent<Monster>(); 
		if (monster->attackType != eAttackType::Melee)
		{
			continue;
		}

		if (monster->state != Monster::eState::Attack)
		{
			continue;
		}

		const Animator* anim = monsterEntity->GetComponent<Animator>();
		Active* active = attackEntity->GetComponent<Active>();
		const ColliderState* state = monsterEntity->GetComponent<ColliderState>();

		if (anim->frameIndex >= state->attackAnimIndex + 1
			or anim->frameIndex < state->attackAnimIndex)
		{
			active->isValue = false;
			continue;
		}

		active->isValue = true;

		BoxCollider* boxCollider = attackEntity->GetComponent<BoxCollider>();
		boxCollider->size = state->attackSize;
		boxCollider->offset = state->attackOffset;

		const Direction* monsterDirection = monsterEntity->GetComponent<Direction>();
		Direction* direction = attackEntity->GetComponent<Direction>();
		direction->value = monsterDirection->value;

		const Transform* monsterTransform = monsterEntity->GetComponent<Transform>();
		Transform* transform = attackEntity->GetComponent<Transform>();
		transform->position = monsterTransform->position;
		transform->scale = monsterTransform->scale;

		const Damage* damage = monsterEntity->GetComponent<Damage>();
		Damage* attackDamage = attackEntity->GetComponent<Damage>();
		attackDamage->value = damage->value;
	}
}

void MainScene::removeAttackCollider()
{
	const std::vector<Entity*> monsterEntities = getEntities<MonsterTag>();
	for (const Entity* monsterEntity : monsterEntities)
	{
		if (not monsterEntity->HasComponent<Monster>())
		{
			continue;
		}

		const Monster* monster = monsterEntity->GetComponent<Monster>();
		if (monster->attackType != eAttackType::Melee)
		{
			continue;
		}

		if (monster->state != Monster::eState::Attack)
		{
			continue;
		}

		const Animator* anim = monsterEntity->GetComponent<Animator>();
		const ColliderState* state = monsterEntity->GetComponent<ColliderState>();

		if (anim->frameIndex == state->attackAnimIndex + 2)
		{
			for (const std::vector<Entity*> entities = getEntities<MonsterAttackColliderTag>();
				Entity* attackEntity : entities)
			{
				if (not attackEntity->HasComponent<MonsterAttackCollider>())
				{
					continue;
				}

				if (Active* active = attackEntity->GetComponent<Active>();
					active->isValue)
				{
					active->isValue = false;
				}

				MonsterAttackCollider* attackCollider = attackEntity->GetComponent<MonsterAttackCollider>();
				if (attackCollider->ownerEntity != monsterEntity)
				{
					continue;
				}

				attackEntity->SetRemove();
				attackEntity->RemovedComponent();

				break;
			}
		}
	}
}

template<typename T>
void MainScene::spawnRangedAttack(const SpawnRangeAttackDesc& desc)
{
	constexpr float monsterLeftOffsetX = 20.0f;
	constexpr float monsterRightOffsetX = 80.0f;
	constexpr Point centerOffset = { .x = -0.4f, .y = 0.0f };

	const eMonsterType type = desc.type;
	Texture* texture = desc.texture;
	const uint32_t spawnFrameIndex = desc.spawnFrameIndex;

	for (const std::vector<Entity*> monsterEntities = getEntities<MonsterTag>();
		const Entity* monsterEntity : monsterEntities)
	{
		if (not monsterEntity->HasComponent<MonsterTag>())
		{
			continue;
		}

		Monster* monster = monsterEntity->GetComponent<Monster>();
		if (monster->type != type)
		{
			continue;
		}

		if (monster->attackType != eAttackType::Range)
		{
			continue;
		}

		const Animator* monsterAnim = monsterEntity->GetComponent<Animator>();
		if (monsterAnim->clipState != &mArcherClips[uint32_t(Monster::eState::Attack)])
		{
			continue;
		}
			
		if (monsterAnim->frameIndex == spawnFrameIndex)
		{
			if (monster->isRangeAttack)
			{
				continue;
			}

			monster->isRangeAttack = true;

			// 기본 세팅을 합니다.
			Entity* arrowEntity = GetEntityWorld()->AddEntity(new Entity());
			{
				arrowEntity->AddComponent(T());
				arrowEntity->AddComponent(MonsterRangedAttackTag());
				arrowEntity->AddComponent(RangedAttack());
				arrowEntity->AddComponent(Direction());
				arrowEntity->AddComponent(Transform());
				arrowEntity->AddComponent(Image());
				arrowEntity->AddComponent(Active());
				arrowEntity->AddComponent(Damage());
				arrowEntity->AddComponent(BoxCollider());
				arrowEntity->AddComponent(DebugActive());
				arrowEntity->AddComponent(DebugColor());

				RangedAttack* rangedAttack = arrowEntity->GetComponent<RangedAttack>();
				rangedAttack->distance = monster->attackDistance;
				rangedAttack->isFiring = true;

				const Direction* monsterDirection = monsterEntity->GetComponent<Direction>();
				Direction* direction = arrowEntity->GetComponent<Direction>();
				direction->value = monsterDirection->value;

				Transform* transform = arrowEntity->GetComponent<Transform>();
				transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
				transform->angle = 90.0f;
				transform->flip = (direction->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

				float degree = std::atan2(direction->value.y, direction->value.x) * (180.0f / 3.141592f);
				degree -= 90.0f;
				transform->angle = -degree;

				Image* image = arrowEntity->GetComponent<Image>();
				image->texture = texture;

				const Damage* monsterDamage = monsterEntity->GetComponent<Damage>();
				Damage* damage = arrowEntity->GetComponent<Damage>();
				damage->value = monsterDamage->value;

				Active* active = arrowEntity->GetComponent<Active>();
				active->isValue = true;

				CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Arrow));
				collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Player));
				arrowEntity->AddComponent(collider);

				BoxCollider* boxCollider = arrowEntity->GetComponent<BoxCollider>();
				boxCollider->size = { .width = float(mArrowTexture.GetWidth()), .height = float(mArrowTexture.GetHeight()) };
			}

			// Offset을 계산한다.
			const Transform* monsterTransform = monsterEntity->GetComponent<Transform>();
			const float centerOffsetX = centerOffset.x * (mArrowTexture.GetWidth() - 1.0f);

			RangedAttack* rangedAttack = arrowEntity->GetComponent<RangedAttack>();
			Transform* transform = arrowEntity->GetComponent<Transform>();

			if (transform->flip == SDL_FLIP_NONE)
			{
				rangedAttack->startPosition.x = monsterTransform->position.x
					+ (centerOffsetX - mArrowTexture.GetWidth()) * mArrowTexture.GetWidth() + monsterRightOffsetX;

				transform->position.x = monsterTransform->position.x
					+ (centerOffsetX - mArrowTexture.GetWidth()) * mArrowTexture.GetWidth() + monsterRightOffsetX;
			}
			else
			{
				rangedAttack->startPosition.x = monsterTransform->position.x
					+ (centerOffsetX - mArrowTexture.GetWidth()) * mArrowTexture.GetWidth() - monsterLeftOffsetX;

				transform->position.x = monsterTransform->position.x
					+ (centerOffsetX - mArrowTexture.GetWidth()) * mArrowTexture.GetWidth() - monsterLeftOffsetX;
			}

			rangedAttack->startPosition.y = monsterTransform->position.y;
			transform->position.y = monsterTransform->position.y;

			break;		
		}
		else if (monsterAnim->frameIndex >= monsterAnim->clipState->GetLastFrameIndex() - 1)
		{
			const Entity* playerEntity = getEntity<PlayerTag>();
			const Transform* playerTransform = playerEntity->GetComponent<Transform>();

			Transform* monsterTransform = monsterEntity->GetComponent<Transform>();
			const Point difference = playerTransform->position - monsterTransform->position;
			
			Direction* direction = monsterEntity->GetComponent<Direction>();
			direction->value = Math::NormalizeVector(difference);
			
			monsterTransform->flip = (direction->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

			monster->isRangeAttack = false;
		}
		else
		{
			monster->isRangeAttack = false;
		}
	}
}

template<typename T>
void MainScene::rangedAttackState()
{
	for (const auto entities = getEntities<T>();
		Entity* entity : entities)
	{
		if (Active* active = entity->GetComponent<Active>();
			not active->isValue)
		{
			continue;
		}

		RangedAttack* rangedAttack = entity->GetComponent<RangedAttack>();
		if (not rangedAttack->isFiring)
		{
			continue;
		}

		Transform* transform = entity->GetComponent<Transform>();
		if (Math::GetVectorLength(transform->position - rangedAttack->startPosition) >= rangedAttack->distance)
		{
			rangedAttack->isFiring = false;
			entity->SetRemove();
		}
	}
}

void MainScene::rangedAttackMove(const float speed, const float deltaTime)
{
	for (const auto entities = getEntities<MonsterRangedAttackTag>();
		Entity* entity : entities)
	{
		if (Active* active = entity->GetComponent<Active>(); 
			not active->isValue)
		{
			continue;
		}

		const Direction* direction = entity->GetComponent<Direction>();
		const Point velocity = direction->value * speed;

		Transform* transform = entity->GetComponent<Transform>();
		transform->position = transform->position + velocity * deltaTime;
	}
}

void MainScene::playerToMonsterCollision()
{
	const std::vector<Entity*> monsterEntities = getEntities<MonsterTag>();
	for (const Entity* monsterEntity : monsterEntities)
	{
		if (monsterEntity->HasComponent<Active>())
		{
			if (const Active* active = monsterEntity->GetComponent<Active>();
				not active->isValue)
			{
				continue;
			}
		}

		const Entity* playerEntity = getEntity<PlayerTag>();
		if (isCollisionEnter(*playerEntity, *monsterEntity))
		{
			Hp* playerHp = playerEntity->GetComponent<Hp>();
			const Damage* damage = monsterEntity->GetComponent<Damage>();
			playerHp->value -= damage->value;

			std::string name = "Hp: " + std::to_string(playerHp->value);

			const Entity* hpEntity = getEntity<PlayerHpBarTag>();
			Label* playerLabel = hpEntity->GetComponent<Label>();
			playerLabel->text = name;
		}
		else if (isCollisionStay(*playerEntity, *monsterEntity))
		{
			Knockback* knockback = playerEntity->GetComponent<Knockback>();
			knockback->isValue = true;
		}
	}
}

void MainScene::playerToMonsterAttackCollision()
{
	for (const std::vector<Entity*> entities = getEntities<MonsterAttackColliderTag>();
		Entity * attackEntity : entities)
	{
		if (attackEntity->HasComponent<Active>())
		{
			if (const Active* active = attackEntity->GetComponent<Active>();
				not active->isValue)
			{
				continue;
			}
		}

		const Entity* playerEntity = getEntity<PlayerTag>();
		if (isCollisionEnter(*playerEntity, *attackEntity))
		{
			Hp* playerHp = playerEntity->GetComponent<Hp>();
			const Damage* damage = attackEntity->GetComponent<Damage>();
			playerHp->value -= damage->value;

			std::string name = "Hp: " + std::to_string(playerHp->value);
			const Entity* hpEntity = getEntity<PlayerHpBarTag>();
			Label* playerLabel = hpEntity->GetComponent<Label>();
			playerLabel->text = name;
		}
		else if (isCollisionStay(*playerEntity, *attackEntity))
		{
			Knockback* knockback = playerEntity->GetComponent<Knockback>();
			knockback->isValue = true;
		}
	}
}

void MainScene::playerToArrowCollision()
{
	for (const auto entities = getEntities<MonsterArrowTag>();
		Entity* arrowEntity : entities)
	{
		if (arrowEntity->HasComponent<Active>())
		{
			if (const Active* active = arrowEntity->GetComponent<Active>();
				not active->isValue)
			{
				continue;
			}
		}

		const Entity* playerEntity = getEntity<PlayerTag>();
		if (isCollisionEnter(*playerEntity, *arrowEntity))
		{
			Hp* playerHp = playerEntity->GetComponent<Hp>();
			const Damage* damage = arrowEntity->GetComponent<Damage>();
			playerHp->value -= damage->value;

			std::string name = "Hp: " + std::to_string(playerHp->value);
			const Entity* hpEntity = getEntity<PlayerHpBarTag>();
			Label* playerLabel = hpEntity->GetComponent<Label>();
			playerLabel->text = name;
		}
		else if (isCollisionStay(*playerEntity, *arrowEntity))
		{
			Knockback* knockback = playerEntity->GetComponent<Knockback>();
			knockback->isValue = true;

			Active* active = arrowEntity->GetComponent<Active>();
			active->isValue = false;

			RangedAttack* arrow = arrowEntity->GetComponent<RangedAttack>();
			arrow->isFiring = false;
		}
	}
}

void MainScene::swordSkillToMonsterCollision()
{
	const std::vector<Entity*> monsterEntities = getEntities<MonsterTag>();
	for (Entity* monsterEntity : monsterEntities)
	{
		if (const Active* active = monsterEntity->GetComponent<Active>();
			not active)
		{
			continue;
		}

		const Monster::eState monsterState = monsterEntity->GetComponent<Monster>()->state;
		if (monsterState != Monster::eState::Run
			and monsterState != Monster::eState::Attack)
		{
			continue;
		}

		const Entity* swordSkillEntity = getEntity<SwordSkillTag>();
		if (isCollisionEnter(*swordSkillEntity, *monsterEntity))
		{
			Hp* hp = monsterEntity->GetComponent<Hp>();
			hp->value -= 1;
			
			const Entity* hpBarEntity = getEntities<MonsterHpBarTag>()[hp->hpBarIndex];
			Transform* hpBarTransform = hpBarEntity->GetComponent<Transform>();
			const float currentWidth = (static_cast<float>(hp->value) / hp->max) * 0.8f;
			hpBarTransform->scale.width = currentWidth;
		}
		else if (isCollisionStay(*swordSkillEntity, *monsterEntity))
		{
			Knockback* knockback = monsterEntity->GetComponent<Knockback>();
			knockback->isValue = true;
		}
	}
}

void MainScene::bulletToMonsterCollision()
{
	const std::vector<Entity*> monsterEntities = getEntities<MonsterTag>();
	for (Entity* monsterEntity : monsterEntities)
	{
		if (const Active* active = monsterEntity->GetComponent<Active>();
			not active)
		{
			continue;
		}

		const Monster::eState monsterState = monsterEntity->GetComponent<Monster>()->state;
		if (monsterState != Monster::eState::Run
			and monsterState != Monster::eState::Attack)
		{
			continue;
		}

		const std::vector<Entity*> entities = getEntities<BulletTag>();
		for (Entity* bulletEntity : entities)
		{
			if (isCollisionEnter(*bulletEntity, *monsterEntity))
			{
				Hp* hp = monsterEntity->GetComponent<Hp>();
				hp->value -= 2;

				const Entity* hpBarEntity = getEntities<MonsterHpBarTag>()[hp->hpBarIndex];
				Transform* transform = hpBarEntity->GetComponent<Transform>();
				const float currentWidth = (static_cast<float>(hp->value) / hp->max) * 0.8f;
				transform->scale.width = currentWidth;

				Active* bulletActive = bulletEntity->GetComponent<Active>();
				bulletActive->isValue = false;
			}
			else if (isCollisionStay(*bulletEntity, *monsterEntity))
			{
				Knockback* knockback = monsterEntity->GetComponent<Knockback>();
				knockback->isValue = true;
			}
		}
	}
}

void MainScene::clampToTile(Transform* transform, const Range rangeX, const Range RangeY)
{
	assert(transform != nullptr);

	const Point firstTilePosition = mTiles[0][0].GetComponent<Transform>()->position;
	const Point lastTilePosition = mTiles[mTileMaxCount - 1][mTileMaxCount - 1].GetComponent<Transform>()->position;

	transform->position.x = std::clamp(transform->position.x, firstTilePosition.x + rangeX.min, lastTilePosition.x - rangeX.max);
	transform->position.y = std::clamp(transform->position.y, lastTilePosition.y + RangeY.min, firstTilePosition.y - RangeY.max);
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

	const Entity* entity = getEntity<Camera>();
	screenPosition += entity->GetComponent<Transform>()->position;

	return screenPosition;
}

void MainScene::initializeGun()
{
	constexpr Point CENTER = { .x = -0.25f,.y = -0.25f };

	const Entity* entity = getEntity<GunTag>();

	Transform* transform = entity->GetComponent<Transform>();
	transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
	transform->center = CENTER;

	Image* image = entity->GetComponent<Image>();
	image->texture = &mGunTexture;

	Active* active = entity->GetComponent<Active>();
	active->isValue = true;
}

void MainScene::updateGun()
{
	constexpr Point OFFSET =
	{
		.x = 16.0f,
		.y = 14.0f
	};

	const Entity* entity = getEntity<GunTag>();
	Transform* transform = entity->GetComponent<Transform>();
	Direction* direction = entity->GetComponent<Direction>();

	const Entity* playerEntity = getEntity<PlayerTag>();
	const Transform* playerTransform = playerEntity->GetComponent<Transform>();
	const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;

	float degree = std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);
	transform->position = playerTransform->position;
	transform->flip = playerTransform->flip;

	transform->position.y += OFFSET.y;

	if (transform->flip == SDL_FLIP_HORIZONTAL)
	{
		degree += 180.0f;
		transform->position.x -= OFFSET.x;
	}
	else
	{
		transform->position.x += OFFSET.x;
	}

	transform->angle = -degree;

	const float length = Math::GetVectorLength(mouseToPlayer);
	direction->value = mouseToPlayer / length;
}

void MainScene::initializeSword()
{
	const Entity* entity = getEntity<SwordTag>();

	Transform* transform = entity->GetComponent<Transform>();
	transform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
	transform->center = { .x = 0.0f,.y = -0.33f };

	Animator* animator = entity->GetComponent<Animator>();
	animator->clipState = &mSwordClip;

	Active* active = entity->GetComponent<Active>();
	active->isValue = true;
}

void MainScene::updateSword()
{		
	const Entity* swordSkillEntity = getEntity<SwordSkillTag>();
	if (const Active* active = swordSkillEntity->GetComponent<Active>();
		active->isValue)
	{
		return;
	}

	const Entity* playerEntity = getEntity<PlayerTag>();
	const Transform* playerTransform = playerEntity->GetComponent<Transform>();

	const Point offset =
	{
		.x = (playerTransform->flip == SDL_FLIP_NONE) ? -60.0f : 60.0f,
		.y = 80.0f
	};

	const Entity* entity = getEntity<SwordTag>();
	const Point targetPosition = offset + playerTransform->position;

	Transform* transform = entity->GetComponent<Transform>();
	transform->position = Math::LerpVector(transform->position, targetPosition, 0.16f);
	transform->angle = 0.0f;
}

void MainScene::updateSwordStates(const float deltaTime)
{
	constexpr float ANGLE_OFFSET = 90.0f;
	constexpr float END_SWING_TIME = 1.0f;

	const Entity* swordSkillEntity = getEntity<SwordSkillTag>();
	if (const Active* active = swordSkillEntity->GetComponent<Active>();
		not active->isValue)
	{
		return;
	}

	const Entity* entity = getEntity<SwordTag>();

	const Transform* skillTransform = swordSkillEntity->GetComponent<Transform>();
	const Direction* skillDirection = swordSkillEntity->GetComponent<Direction>();
	const Point velocity = skillDirection->value * 20.0f;

	Transform* transform = entity->GetComponent<Transform>();
	transform->angle = skillTransform->angle;	
	transform->position = skillTransform->position + velocity * deltaTime;
	transform->flip = (skillDirection->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
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

Quad MainScene::convertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const
{
	const Point position = transform.position + boxCollider.offset;
	const Scale boxHalfSize = transform.scale * boxCollider.size * 0.5f;

	const Quad local =
	{
		.leftTop = { -boxHalfSize.width, -boxHalfSize.height },
		.rightTop = { boxHalfSize.width, -boxHalfSize.height }, 
		.leftBottom = { -boxHalfSize.width, boxHalfSize.height },
		.rightBottom = { boxHalfSize.width, boxHalfSize.height }
	};

	const Quad rotate =
	{
		.leftTop = Math::RotatePoint(local.leftTop, -transform.angle),
		.rightTop = Math::RotatePoint(local.rightTop, -transform.angle), 
		.leftBottom = Math::RotatePoint(local.leftBottom, -transform.angle), 
		.rightBottom = Math::RotatePoint(local.rightBottom, -transform.angle)
	};

	const Quad result
	{
		.leftTop = {.x = position.x + rotate.leftTop.x, .y = position.y - rotate.leftTop.y },
		.rightTop = { .x = position.x + rotate.rightTop.x, .y = position.y - rotate.rightTop.y },
		.leftBottom = { .x = position.x + rotate.leftBottom.x, .y = position.y - rotate.leftBottom.y },
		.rightBottom = { .x = position.x + rotate.rightBottom.x, .y = position.y - rotate.rightBottom.y }
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
	const Quad quad0 = convertBoxColliderToWorldBox(*transform0, *boxCollider0);

	const Transform* transform1 = entity1.GetComponent<Transform>();
	const BoxCollider* boxCollider1 = entity1.GetComponent<BoxCollider>();
	const Quad quad1 = convertBoxColliderToWorldBox(*transform1, *boxCollider1);

	if (Collision::IsCollidedSqureWithSqure(quad0, quad1))
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

	// TODO: Quad를 사용해서 수정하기
	const Transform* boxTransform = boxEntity.GetComponent<Transform>();
	const BoxCollider* boxCollider = boxEntity.GetComponent<BoxCollider>();
	//const std::array<Point, 5> points = convertBoxColliderToWorldBox(*boxTransform, *boxCollider);

	const Transform* circleTransform = circleEntity.GetComponent<Transform>();
	const CircleCollider* circleCollider = circleEntity.GetComponent<CircleCollider>();
	const Circle circle = convertCircleColliderToWorldCircle(*circleTransform, *circleCollider);

	//if (Collision::IsCollidedSqureWithCircle(rect, circle))
	//{
	//	registerCollidedEntityPairs(boxEntity, circleEntity);
	//	return true;
	//}

	return false;
}

bool MainScene::checkCollisionBoxLine(const Entity& boxEntity, const Entity& lineEntity)
{
	if (not boxEntity.HasComponent<BoxCollider>()
		or not lineEntity.HasComponent<LineCollider>())
	{
		return false;
	}

	// TODO: Quad를 사용해서 수정하기
	//const Transform* boxTransform = boxEntity.GetComponent<Transform>();
	//const BoxCollider* boxCollider = boxEntity.GetComponent<BoxCollider>();
	//const Rect rect = convertBoxColliderToWorldBox(*boxTransform, *boxCollider);

	//const Transform* lineTransform = lineEntity.GetComponent<Transform>();
	//const LineCollider* lineCollider = lineEntity.GetComponent<LineCollider>();
	//const Line line = convertLineColliderToWorldLine(*lineTransform, *lineCollider);

	//if (Collision::IsCollidedSqureWithLine(rect, line))
	//{
	//	registerCollidedEntityPairs(boxEntity, lineEntity);
	//	return true;
	//}

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