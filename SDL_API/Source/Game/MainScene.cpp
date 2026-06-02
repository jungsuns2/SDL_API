#include "pch.h"
#include "Core/Entity/EntityWorld.h"

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

	// 현재 웨이브 상태를 초기화한다.
	mGameWaveState.remainingMonsterGroupSpawnTime = WAVES[mGameWaveState.index].monsterGroupSpawnIntervalTime;

	initializeMonsters();
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

	input();

	// Wave를 업데이트한다.
	{
		// 현재 웨이브 상태를 업데이트한다.
		mGameWaveState.waveTimer += deltaTime;
		if (mGameWaveState.waveTimer >= WAVES[mGameWaveState.index].elapsedTime)
		{
			if (++mGameWaveState.index >= WAVES.size())
			{
				assert(false && "구현 예정.");
			}

			mGameWaveState.waveTimer = 0.0f;
			mGameWaveState.groupIndex = 0;
			mGameWaveState.remainingMonsterGroupSpawnTime = WAVES[mGameWaveState.index].monsterGroupSpawnIntervalTime;
			mGameWaveState.labelShowElapsedTime = 0.0f;

			// 다음 웨이브를 위해 몬스터와 연관된 것들을 초기화한다.
			for (Entity& entity : mMonsters)
			{
				Monster* monster = entity.GetComponent<Monster>();
				monster->state = Monster::eState::Spawn;

				Active* active = entity.GetComponent<Active>();
				active->isValue = false;
			}

			for (Entity& entity : mMonsterHpBars)
			{
				Active* active = entity.GetComponent<Active>();
				active->isValue = false;
			}

			for (Entity& entity : mArrows)
			{
				Active* active = entity.GetComponent<Active>();
				active->isValue = false;
			}

			// 라벨 정보를 갱신한다.
			{
				Label* label = mStageLabel.GetComponent<Label>();
				const std::string name = std::to_string(mGameWaveState.index + 1) + " Wave";
				label->text = name;

				Active* active = mStageLabel.GetComponent<Active>();
				active->isValue = true;
			}
		}

		{
			const WaveDesc& waveDesc = WAVES[mGameWaveState.index];
			const uint32_t monsterGroupIndex = waveDesc.monsterGroupIndicies[mGameWaveState.groupIndex];
			const MonsterGroup& monsterGroup = MONSTER_GROUPS[monsterGroupIndex];

			mGameWaveState.remainingMonsterGroupSpawnTime -= deltaTime;
			if (mGameWaveState.remainingMonsterGroupSpawnTime <= 0.0f)
			{
				spawnMonsterGroup(monsterGroup);

				++mGameWaveState.groupIndex;
				assert(mGameWaveState.groupIndex < MONSTER_GROUPS.size() && "더 이상 그룹이 없습니다.");

				mGameWaveState.remainingMonsterGroupSpawnTime = waveDesc.monsterGroupSpawnIntervalTime;
			}
		}

		// 다음 웨이브가 되면 잠시 동안 라벨을 표시한다.
		{
			mGameWaveState.labelShowElapsedTime += deltaTime;

			Active* active = mStageLabel.GetComponent<Active>();
			if (active->isValue and mGameWaveState.labelShowElapsedTime >= 2.0f)
			{
				mGameWaveState.labelShowElapsedTime = 0.0f;
				active->isValue = false;
			}
		}

		// Upeate Wave Timer Label
		{
			Label* label = mWaveTimerLebel.GetComponent<Label>();
			const uint32_t seconds = uint32_t(mGameWaveState.waveTimer) % 60;
			const uint32_t minutes = uint32_t(mGameWaveState.waveTimer) / 60;

			const std::string fseconds = (seconds < 10) ? "0" + std::to_string(seconds) : std::to_string(seconds);
			const std::string fMinutes = (minutes < 10) ? "0" + std::to_string(minutes) : std::to_string(minutes);

			const std::string name = "Timer: " + fMinutes + ":" + fseconds;
			label->text = name;
		}
	}

	// 플레이어를 업데이트한다.
	{
		playerState(deltaTime);

		playerMove(deltaTime);
	}

	// 카메라를 업데이트한다.
	{
		Transform* transform = mMainCamera.GetComponent<Transform>();
		Transform* target = mPlayer.GetComponent<Transform>();
		transform->position = target->position;

		constexpr Point OFFSET = { .x = 24.0f, .y = 23.0f };
		const Scale halfScreen =
		{
			.width = Constant::Get().GetHalfWidth() - OFFSET.x,
			.height = Constant::Get().GetHalfHeight() - OFFSET.y,
		};

		clampToTile(transform, { .min = halfScreen.width, .max = halfScreen.width }, { .min = halfScreen.height, .max = halfScreen.height });
	}

	// 칼 이동을 업데이트한다.
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

	// 칼 이펙트를 스폰한다.
	{
		Active* active = mSwordSkill.GetComponent<Active>();
		if (Effect* effect = mSwordSkill.GetComponent<Effect>();
			Input::Get().GetMouseButtonDown(SDL_BUTTON_LEFT)
			and not active->isValue
			and not effect->isDisabled)
		{
			Animator* effectAnim = mSwordSkill.GetComponent<Animator>();
			effectAnim->frameIndex = 0;
			effectAnim->elapsedTime = 0.0f;

			active->isValue = true;

			const Transform* playerTransform = mPlayer.GetComponent<Transform>();

			RangedAttack* rangedAttack = mSwordSkill.GetComponent<RangedAttack>();
			rangedAttack->startPosition = playerTransform->position;

			Transform* effectTransform = mSwordSkill.GetComponent<Transform>();
			effectTransform->position = playerTransform->position;

			const Point mouseToPlayer = getScreenMousePosition() - playerTransform->position;
			Direction* effectDirection = mSwordSkill.GetComponent<Direction>();
			effectDirection->value = Math::NormalizeVector(mouseToPlayer);

			float degree = std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);
			degree -= 90.0f;
			effectTransform->angle = -degree;

			effectTransform->flip = (effectDirection->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		}
	}

	// 칼 이펙트를 업데이트한다.
	{
		constexpr float SPEED = 900.0f;
		constexpr float SWING_COOLTIME = 0.7f;

		Direction* direction = mSwordSkill.GetComponent<Direction>();
		const Point velocity = direction->value * SPEED;

		Transform* transform = mSwordSkill.GetComponent<Transform>();
		transform->position = transform->position + velocity * deltaTime;

		Effect* effect = mSwordSkill.GetComponent<Effect>();

		if (Active* active = mSwordSkill.GetComponent<Active>(); 
			active->isValue)
		{			
			if (const RangedAttack* rangedAttack = mSwordSkill.GetComponent<RangedAttack>();
				Math::GetVectorLength(transform->position - rangedAttack->startPosition) >= rangedAttack->distance)
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
		updateMonsterStates(deltaTime);

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

			// 몬스터 죽는 처리 및 파티클이 생성된다.
			//monsterDeadParticle(deltaTime);
		}

		// TODO: 몬스터 종류(archer, skel)마다 속도가 결정되도록 수정이 필요하다.
		constexpr float SPEED = 35.0f;
		monsterMove(SPEED, deltaTime);

		monsterHpBarMove();
	}

	// 원거리 몬스터의 공격을 초기화와 업데이트한다.
	{
		spawnRangedAttack(mArrows, eMonsterType::Archer, 7);
		
		rangedAttackState(mArrows);

		rangedAttackMove(mArrows, 300.0f, deltaTime);
	}

	// 충돌을 업데이트한다.
	{
		// 몬스터와 플레이어
		for (const Entity& monsterEntity : mMonsters)
		{
			if (not monsterEntity.GetComponent<Active>()->isValue)
			{
				continue;
			}

			Monster::eState monsterState = monsterEntity.GetComponent<Monster>()->state;
			if (monsterState == Monster::eState::Spawn)
			{
				continue;
			}

			if (isCollisionEnter(mPlayer, monsterEntity))
			{
				Hp* playerHp = mPlayer.GetComponent<Hp>();
				playerHp->value -= 1;

				std::string name = "Hp: " + std::to_string(playerHp->value);
				Label* playerLabel = mUIPlayerHp.GetComponent<Label>();
				playerLabel->text = name;
			}
			else if (isCollisionStay(mPlayer, monsterEntity))
			{
				if (monsterState == Monster::eState::Attack)
				{
					Knockback* knockback = mPlayer.GetComponent<Knockback>();
					knockback->isValue = true;
				}
			}
		}

		// 화살과 플레이어
		for (const Entity& arrowEntity : mArrows)
		{
			if (Active* active = arrowEntity.GetComponent<Active>();
				not active->isValue)
			{
				continue;
			}

			if (isCollisionEnter(mPlayer, arrowEntity))
			{
				Hp* playerHp = mPlayer.GetComponent<Hp>();
				playerHp->value -= 1;

				std::string name = "Hp: " + std::to_string(playerHp->value);
				Label* playerLabel = mUIPlayerHp.GetComponent<Label>();
				playerLabel->text = name;
			}
			else if (isCollisionStay(mPlayer, arrowEntity))
			{
				Active* active = arrowEntity.GetComponent<Active>();
				active->isValue = false;

				RangedAttack* arrow = arrowEntity.GetComponent<RangedAttack>();
				arrow->isFire = false;
			}
		}

		// 몬스터와 플레이어 칼 이펙트
		swordSkillToMonsterCollision();

		mPreviousCollidedEntityPairs = mCollidedEntityPairs;
		mCollidedEntityPairs.clear();
	}

	playerSetClip();
	monsterSetClip();

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

		mPlayerHandTexture.Finalize();
		mPlayerDeadTexture.Finalize();
	}

	// Monster
	{
		mArrowTexture.Finalize();

		for (Texture& texture : mBigWhiteSkelIdleTextures)
		{
			texture.Finalize();
		}

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

		constexpr Point IDLE_OFFSETS[] =
		{
			{.x = 0.07f, .y = -0.5f },
			{.x = 0.07f, .y = -0.5f },
			{.x = 0.07f, .y = -0.5f },
			{.x = 0.07f, .y = -0.5f },
			{.x = 0.07f, .y = -0.5f },
		};
		assert(std::size(IDLE_OFFSETS) == mBigWhiteSkelIdleTextures.size() and "offset 개수와 texture 개수가 일치하지 않습니다.");

		// Idle
		for (Texture& texture : mBigWhiteSkelIdleTextures)
		{
			texture.Initialize(GetHelper(), "Resource/Monster/BigWhiteSekl/Idle/" + std::to_string(index) + ".png");

			Clip::Frame frame =
			{
				.texture = &texture,
				.durationTime = 0.12f,
				.center = IDLE_OFFSETS[index]
			};

			mBigWhiteSkelClips[uint32_t(Monster::eState::Idle)].AddClip(frame);
			++index;
		}

		index = 0;

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

		// Run
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

		// Attack
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

		mArrowTexture.Initialize(GetHelper(), "Resource/Monster/Archer/Arrow/0.png");
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
		// Timer
		{
			UI ui{};
			mWaveTimerLebel.AddComponent(ui);

			Label label{};
			label.font = &mUIFont;
			label.text = "dd";
			mWaveTimerLebel.AddComponent(label);

			Transform transform{};
			transform.position = { .x = Constant::Get().GetHalfWidth() - 150.0f, .y = 0.0f };
			mWaveTimerLebel.AddComponent(transform);

			Active active{};
			active.isValue = true;
			mWaveTimerLebel.AddComponent(active);

			GetEntityWorld()->AddEntity(&mWaveTimerLebel);
		}

		// Wave Stage
		{
			UI ui{};
			mStageLabel.AddComponent(ui);

			Label label{};
			label.font = &mUIFont;
			label.text = "1 Wave";
			mStageLabel.AddComponent(label);

			Transform transform{};
			transform.position = { .x = Constant::Get().GetHalfWidth() - 80.0f, .y = Constant::Get().GetHalfHeight() - 50.0f };
			mStageLabel.AddComponent(transform);

			Active active{};
			active.isValue = true;
			mStageLabel.AddComponent(active);

			GetEntityWorld()->AddEntity(&mStageLabel);
		}
	}

	// Gun
	{
		constexpr Point CENTER = { .x = -0.25f,.y = -0.25f };

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

	// Player Left Hand
	{
		Transform transform{};
		transform.scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
		mPlayerLeftHand.AddComponent(transform);

		Image image{};
		image.texture = &mPlayerHandTexture;
		mPlayerLeftHand.AddComponent(image);

		Active active{};
		active.isValue = true;
		mPlayerLeftHand.AddComponent(active);

		GetEntityWorld()->AddEntity(&mPlayerLeftHand);
	}

	// Player
	{
		constexpr uint32_t MAX_HP = 100;

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
		hp.max = MAX_HP;
		hp.value = hp.max;
		mPlayer.AddComponent(hp);

		Knockback knockback{};
		mPlayer.AddComponent(knockback);

		Active active{};
		active.isValue = true;
		mPlayer.AddComponent(active);

		Color color{};
		mPlayer.AddComponent(color);

		CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Player));
		collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Monster));
		mPlayer.AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.offset = { .x = 0.0f, .y = 32.0f };
		boxCollider.size = { .width = float(mPlayerRunTextures[3].GetWidth()), .height = float(mPlayerRunTextures[3].GetHeight()) };
		mPlayer.AddComponent(boxCollider);

		DebugActive debugActive{};
		mPlayer.AddComponent(debugActive);

		DebugColor debugColor{};
		mPlayer.AddComponent(debugColor);

		GetEntityWorld()->AddEntity(&mPlayer);
	}

	// Player Hp
	{
		UI ui{};
		mUIPlayerHp.AddComponent(ui);

		Label label{};
		label.font = &mUIFont;
		const Hp* hp = mPlayer.GetComponent<Hp>();
		std::string hpName = "HP: " + std::to_string(hp->value);
		label.text = hpName;
		mUIPlayerHp.AddComponent(label);

		Transform transform{};
		mUIPlayerHp.AddComponent(transform);

		Active active{};
		active.isValue = true;
		mUIPlayerHp.AddComponent(active);

		Color color{};
		mUIPlayerHp.AddComponent(color);

		GetEntityWorld()->AddEntity(&mUIPlayerHp);
	}

	// Player Right Hand
	{
		Transform transform{};
		transform.scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
		mPlayerRightHand.AddComponent(transform);

		Image image{};
		image.texture = &mPlayerHandTexture;
		mPlayerRightHand.AddComponent(image);

		Active active{};
		active.isValue = true;
		mPlayerRightHand.AddComponent(active);

		GetEntityWorld()->AddEntity(&mPlayerRightHand);
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

		RangedAttack rangedAttack{};
		rangedAttack.distance = 200.0f;
		mSwordSkill.AddComponent(rangedAttack);

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

		CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::SwordSkill));
		collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Monster));
		mSwordSkill.AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.size = { .width = float(mSwordSkillTextures[5].GetWidth()), .height = float(mSwordSkillTextures[5].GetHeight()) };
		mSwordSkill.AddComponent(boxCollider);

		DebugActive debugActive{};
		mSwordSkill.AddComponent(debugActive);

		DebugColor debugColor{};
		mSwordSkill.AddComponent(debugColor);

		GetEntityWorld()->AddEntity(&mSwordSkill);
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

		CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Bullet));
		collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Monster));
		mBullet.AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.size = { .width = float(mBulletTextures[5].GetWidth()), .height = float(mBulletTextures[5].GetHeight()) };
		mBullet.AddComponent(boxCollider);

		DebugActive debugActive{};
		mBullet.AddComponent(debugActive);

		DebugColor debugColor{};
		mBullet.AddComponent(debugColor);

		GetEntityWorld()->AddEntity(&mBullet);
	}

	// Monster
	{
		mBigWhiteSkelClips[uint32_t(Monster::eState::Spawn)].SetLoop(true);
		mBigWhiteSkelClips[uint32_t(Monster::eState::Idle)].SetLoop(true);
		mBigWhiteSkelClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mBigWhiteSkelClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		mArcherClips[uint32_t(Monster::eState::Spawn)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Idle)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Attack)].SetLoop(true);
	}

	// Arrow
	{
		for (Entity& entity : mArrows)
		{
			RangedAttack arrow{};
			arrow.distance = 300.0f;
			entity.AddComponent(arrow);

			Transform transform{};
			transform.scale = { .width = 2.5f, .height = 2.5f };
			transform.angle = 90.0f;
			entity.AddComponent(transform);

			Direction direction{};
			entity.AddComponent(direction);

			Active active{};
			entity.AddComponent(active);

			Image image{};
			image.texture = &mArrowTexture;
			entity.AddComponent(image);

			CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Arrow));
			collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Player));
			entity.AddComponent(collider);

			BoxCollider boxCollider{};
			boxCollider.size = { .width = float(mArrowTexture.GetWidth()), .height = float(mArrowTexture.GetHeight()) };
			entity.AddComponent(boxCollider);

			DebugActive debugActive{};
			entity.AddComponent(debugActive);

			DebugColor debugColor{};
			entity.AddComponent(debugColor);

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
			image.texture = &mRedRectTexture;
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

#if defined(_DEBUG)
	if (Input::Get().GetKeyDown(SDL_SCANCODE_T))
	{
		for (Entity* entity : GetEntityWorld()->GetAllEntites())
		{
			if (not entity->HasComponent<DebugActive>())
			{
				continue;
			}

			DebugActive* debugActive = entity->GetComponent<DebugActive>();
			debugActive->isValue = !debugActive->isValue;
		}
	}
#endif
}

void MainScene::playerState(const float deltaTime)
{
	Player* player = mPlayer.GetComponent<Player>();
	player->state = (player->length != 0.0f) ? Player::eState::Run : Player::eState::Idle;

	static float deadTimer;
	Hp* playerHp = mPlayer.GetComponent<Hp>();
	if (playerHp->value <= 0)
	{
		playerHp->value = 0;

		deadTimer += deltaTime;
		if (deadTimer >= 0.5f)
		{
			player->state = Player::eState::Dead;

			Active* leftHandActive = mPlayerLeftHand.GetComponent<Active>();
			leftHandActive->isValue = false;

			Active* rightHandActive = mPlayerRightHand.GetComponent<Active>();
			rightHandActive->isValue = false;
		}
	}
}

void MainScene::playerMove(const float deltaTime)
{
	constexpr float MAX_SPEED = 300.0f;
	constexpr float ACC = 62.0f;

	constexpr float KNOCKBACK_FORCE = 0.0f;
	constexpr float KNOCKBACK_COOLTIMER = 0.7f;

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

	Player* player = mPlayer.GetComponent<Player>();
	player->length = Math::GetVectorLength(moveVelocity);

	Point moveDirection = Math::NormalizeVector(moveVelocity);
	float velocity = fmin(Math::GetVectorLength(moveVelocity), MAX_SPEED);
	moveVelocity = moveDirection * velocity;

	Knockback* knockback = mPlayer.GetComponent<Knockback>();
	Color* color = mPlayer.GetComponent<Color>();

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

	Transform* transform = mPlayer.GetComponent<Transform>();
	clampToTile(transform, { .min = 5.0f, .max = 5.0f }, { .min = -8.0f, .max = 50.0f });
	transform->position += moveVelocity * deltaTime;

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

	// Update Left Hand
	{
		constexpr Point OFFSET = { .x = 14.0f, .y = 18.0f };
		Transform* leftHandTransform = mPlayerLeftHand.GetComponent<Transform>();
		leftHandTransform->position = transform->position;

		leftHandTransform->position.y += OFFSET.y;

		if (transform->flip == SDL_FLIP_NONE)
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
		Transform* rightHandTransform = mPlayerRightHand.GetComponent<Transform>();
		rightHandTransform->position = transform->position;

		rightHandTransform->position.y += OFFSET.y;

		if (transform->flip == SDL_FLIP_NONE)
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

void MainScene::initializeMonsters()
{
	constexpr float SIZE = 0.7f;

	for (Entity& entity : mMonsters)
	{
		Monster monster{};
		entity.AddComponent(monster);

		Transform transform{};
		entity.AddComponent(transform);

		Image image{};
		entity.AddComponent(image);

		Animator animator{};
		entity.AddComponent(animator);

		Direction direction{};
		entity.AddComponent(direction);

		DamageTimer damageTimer{};
		entity.AddComponent(damageTimer);

		// 체력바
		Hp hp{};
		entity.AddComponent(hp);

		Active active{};
		entity.AddComponent(active);

		Color color{};
		entity.AddComponent(color);

		CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Monster));
		collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Monster));
		entity.AddComponent(collider);

		BoxCollider boxCollider{};
		entity.AddComponent(boxCollider);

		ColliderState colliderState{};
		entity.AddComponent(colliderState);

		DebugActive debugActive{};
		entity.AddComponent(debugActive);

		DebugColor debugColor{};
		entity.AddComponent(debugColor);

		GetEntityWorld()->AddEntity(&entity);
	}

	for (Entity& entity : mMonsterHpBars)
	{
		Transform transform{};
		entity.AddComponent(transform);

		Offset offset{};
		entity.AddComponent(offset);

		Image image{};
		image.texture = &mRedRectTexture;
		entity.AddComponent(image);

		Active active{};
		entity.AddComponent(active);

		GetEntityWorld()->AddEntity(&entity);
	}
}

void MainScene::spawnMonsterGroup(const MonsterGroup& group)
{
	constexpr Point TILE_OFFSET = { .x = 30.0f, .y = 50.0f };

	uint32_t monsterIndex = 0;

	// TODO: Rect 멤버변수로 저장해서 처리하자.
	const Point leftTopTilePosition = mTiles[0][0].GetComponent<Transform>()->position;
	const Point rightBottomTilePosition = mTiles[mTileMaxCount - 1][mTileMaxCount - 1].GetComponent<Transform>()->position;

	float groupX = getRandom(leftTopTilePosition.x + TILE_OFFSET.x, rightBottomTilePosition.x - TILE_OFFSET.x);
	float groupY = getRandom(rightBottomTilePosition.y + TILE_OFFSET.y, leftTopTilePosition.y - TILE_OFFSET.y);

	for (uint32_t i = 0; i < group.count; ++i)
	{
		const float xInGroup = getRandom(group.rangeX.min, group.rangeX.max);
		const float yInGroup = getRandom(group.rangeY.min, group.rangeY.max);

		for (__noop; monsterIndex < mMonsters.size(); ++monsterIndex)
		{
			if (const Entity& monsterEntity = mMonsters[monsterIndex]; 
				monsterEntity.GetComponent<Active>()->isValue)
			{
				continue;
			}

			const float monsterX = groupX + xInGroup;
			const float monsterY = groupY + yInGroup;
			spawnMonster
			(
				SpawnMonsterDesc
				{
					.index = monsterIndex,
					.type = group.type,
					.isAttackOption = group.isAttackOption,
					.x = monsterX,
					.y = monsterY
				}
			);

			break;
		}

		++monsterIndex;
	}
}

void MainScene::spawnMonster(const SpawnMonsterDesc& desc)
{
	constexpr float SIZE = 0.7f;

	const uint32_t index = desc.index;
	const eMonsterType type = desc.type;
	const bool isAttackOption = desc.isAttackOption;
	const float x = desc.x;
	const float y = desc.y;

	Entity& monsterEntity = mMonsters[index];
	Entity& hpBarEntity = mMonsterHpBars[index];

	Active* active = monsterEntity.GetComponent<Active>();
	assert(not active->isValue and "이미 사용되고 있는 몬스터입니다.");

	Monster* monster = monsterEntity.GetComponent<Monster>();
	monster->type = type;
	monster->state = Monster::eState::Spawn;

	Transform* transform = monsterEntity.GetComponent<Transform>();
	transform->position = { .x = x, .y = y };
	transform->scale = { .width = SIZE, .height = SIZE };

	DamageTimer* damageTimer = monsterEntity.GetComponent<DamageTimer>();
	damageTimer->deadTimer = 0.0f;

	Hp* hp = monsterEntity.GetComponent<Hp>();
	hp->hpBarIndex = index;

	Active* hpBarActive = hpBarEntity.GetComponent<Active>();
	hpBarActive->isValue = false;

	BoxCollider* boxCollider = monsterEntity.GetComponent<BoxCollider>();
	ColliderState* colliderState = monsterEntity.GetComponent<ColliderState>();
	Offset* hpBarOffset = hpBarEntity.GetComponent<Offset>();

	switch (monster->type)
	{
	case eMonsterType::BigWhite:
		hp->max = 2;
		monster->attackDistance = 90.0f;
		monster->clips = mBigWhiteSkelClips.data();
		colliderState->idleSize = { .width = float(mBigWhiteSkelIdleTextures[2].GetWidth()), .height = float(mBigWhiteSkelIdleTextures[2].GetHeight()) };
		colliderState->runSize = { .width = float(mBigWhiteSkelRunTextures[2].GetWidth()), .height = float(mBigWhiteSkelRunTextures[2].GetHeight()) };
		colliderState->attackSize = { .width = 60.0f, .height = 40.0f };
		colliderState->idleOffset = { .x = 0.0f, .y = 45.0f };
		colliderState->runOffset = { .x = 0.0f, .y = 45.0f };
		colliderState->attackOffset = { .x = 60.0f, .y = 45.0f };
		boxCollider->size = colliderState->runSize;
		boxCollider->offset = colliderState->runOffset;
		hpBarOffset->value = { .x = 0.0f, .y = -20.0f };
		break;

	case eMonsterType::Archer:
		hp->max = 3;
		monster->attackDistance = 300.0f;
		monster->clips = mArcherClips.data();
		colliderState->idleSize = { .width = float(mArcherIdleTextures[1].GetWidth()), .height = float(mArcherIdleTextures[1].GetHeight()) };
		colliderState->runSize = { .width = float(mArcherRunTextures[1].GetWidth()), .height = float(mArcherRunTextures[1].GetHeight()) };
		colliderState->attackSize = { .width = float(mArcherAttackTextures[7].GetWidth()), .height = float(mArcherAttackTextures[7].GetHeight()) };
		boxCollider->size = colliderState->runSize;
		hpBarOffset->value = { .x = 10.0f, .y = -55.0f };
		break;

	default:
		assert(false and "지원하지 않는 몬스터 유형입니다.");
		break;
	}

	monster->isAttackOption = isAttackOption;

	Animator* anim = monsterEntity.GetComponent<Animator>();
	anim->frameIndex = 0;
	anim->elapsedTime = 0.0f;

	hp->value = hp->max;
	active->isValue = true;

	Transform* hpBartransform = hpBarEntity.GetComponent<Transform>();
	const float currentWidth = (static_cast<float>(hp->value) / hp->max) * 0.8f;
	hpBartransform->scale = { .width = currentWidth, .height = 0.2f };
}

void MainScene::updateMonsterStates(const float deltaTime)
{
	for (Entity& entity : mMonsters)
	{
		if (not entity.GetComponent<Active>()->isValue)
		{
			continue;
		}

		Monster* monster = entity.GetComponent<Monster>();
		BoxCollider* boxCollider = entity.GetComponent<BoxCollider>();
		const ColliderState* colliderState = entity.GetComponent<ColliderState>();

		switch (monster->state)
		{
		case Monster::eState::Spawn:
		{
			boxCollider->size = { .width = 0.0f, .height = 0.0f };

			monster->spawnBlinkTimer += deltaTime;
			if (monster->spawnBlinkTimer >= 0.5f)
			{
				Transform* monsterTransform = entity.GetComponent<Transform>();
				monsterTransform->scale = { .width = PRIMARY_SIZE, .height = PRIMARY_SIZE };
				monster->state = Monster::eState::Run;
				monster->spawnBlinkTimer = 0.0f;

				Active* active = entity.GetComponent<Active>();
				active->isValue = true;
			}

			break;
		}

		case Monster::eState::Attack:
		{
			boxCollider->size = colliderState->attackSize;
			boxCollider->offset = colliderState->attackOffset;

			const Direction* direction = entity.GetComponent<Direction>();
			boxCollider->offset.x = (direction->value.x > 0.0f) ? boxCollider->offset.x : -boxCollider->offset.x;

			const Clip& attackClip = monster->clips[uint32_t(Monster::eState::Attack)];
			const Animator& anim = *entity.GetComponent<Animator>();
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

		default:
			assert(false and "지원하지 않는 애니메이션입니다.");
			break;
		}
	}

	for (Entity& monsterEntity : mMonsters)
	{
		Active* monsterActive = monsterEntity.GetComponent<Active>();
		if (not monsterActive)
		{
			continue;
		}

		Hp* hp = monsterEntity.GetComponent<Hp>();
		if (hp->value > 0)
		{
			continue;
		}

		hp->value = 0;
		monsterActive->isValue = false;

		Active* hpBarActive = mMonsterHpBars[hp->hpBarIndex].GetComponent<Active>();
		hpBarActive->isValue = false;
	}
}

void MainScene::monsterDeadParticle(const float deltaTime)
{
	for (Entity& entity : mMonsters)
	{
		if (entity.GetComponent<Active>()->isValue)
		{
			continue;
		}

		Hp* hp = entity.GetComponent<Hp>();
		if (hp->value > 0.0f)
		{
			continue;
		}

		Monster* monster = entity.GetComponent<Monster>();
		Transform* monsterTransform = entity.GetComponent<Transform>();

		for (Entity& deadParticle : mDeadParticle)
		{
			Particle* particle = deadParticle.GetComponent<Particle>();
			Direction* direction = deadParticle.GetComponent<Direction>();
			Point& dir = direction->value;

			dir = getScreenMousePosition() - monsterTransform->position;
			dir = Math::NormalizeVector(dir);
			dir = Math::RotatePoint(dir, getRandom(-60.0f, 60.0f));

			Transform* transform = deadParticle.GetComponent<Transform>();
			transform->position = monsterTransform->position;

			Active* active = entity.GetComponent<Active>();
			active->isValue = true;
		}

		Active* monsterActive = entity.GetComponent<Active>();
		monsterActive->isValue = false;
	}

	for (Entity& entity : mDeadParticle)
	{
		Active* active = entity.GetComponent<Active>();
		if (not active)
		{
			continue;
		}

		DamageTimer* damage = entity.GetComponent<DamageTimer>();
		damage->deadTimer -= deltaTime;

		if (damage->deadTimer <= 0.0f)
		{
			active->isValue = false;
			continue;
		}

		Transform* transform = entity.GetComponent<Transform>();
		Particle* particle = entity.GetComponent<Particle>();
		Direction* direction = entity.GetComponent<Direction>();

		constexpr float SPPED = 3.0f;
		transform->position += direction->value * SPPED * deltaTime;
	}
}

void MainScene::monsterMove(const float maxSpeed, const float deltaTime)
{
	for (Entity& entity : mMonsters)
	{
		Monster* monster = entity.GetComponent<Monster>();
		if (monster->type == eMonsterType::None)
		{
			continue;
		}
		
		if (Active* active = entity.GetComponent<Active>(); 
			not active)
		{
			continue;
		}

		Transform* monsterTransform = entity.GetComponent<Transform>();
		const Transform* playerTransform = mPlayer.GetComponent<Transform>();

		const Point difference = playerTransform->position - monsterTransform->position;
		monster->length = Math::GetVectorLength(difference);

		if (monster->state == Monster::eState::Run)
		{
			Direction* direction = entity.GetComponent<Direction>();
			direction->value = Math::NormalizeVector(difference);

			const Point velocity = direction->value * maxSpeed;

			Knockback* playerKnockback = mPlayer.GetComponent<Knockback>();
			playerKnockback->direction = direction->value;

			clampToTile(monsterTransform, { .min = 5.0f, .max = 5.0f }, { .min = -8.0f, .max = 50.0f });
			monsterTransform->position += velocity * deltaTime;
			monsterTransform->flip = (direction->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		}
	}
}

void MainScene::monsterHpBarMove()
{
	for (const Entity& monsterEntity : mMonsters)
	{
		if (const Active* active = monsterEntity.GetComponent<Active>();
			not active)
		{
			continue;
		}

		if (const Monster* monster = monsterEntity.GetComponent<Monster>();
			monster->state != Monster::eState::Run
			and monster->state != Monster::eState::Attack)
		{
			continue;
		}

		Hp* hp = monsterEntity.GetComponent<Hp>();
		Entity& hpBarEntity = mMonsterHpBars[hp->hpBarIndex];

		Active* hpBarActive = hpBarEntity.GetComponent<Active>();
		hpBarActive->isValue = true;

		setDirectionOffset(&hpBarEntity, monsterEntity);
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
	for (Entity& entity : mMonsters)
	{
		if (not entity.GetComponent<Active>()->isValue)
		{
			continue;
		}

		Monster* monster = entity.GetComponent<Monster>();
		Animator* animator = entity.GetComponent<Animator>();
		switch (monster->state)
		{
		case Monster::eState::Spawn:
			animator->SetClip(&monster->clips[uint32_t(Monster::eState::Spawn)]);
			break;

		case Monster::eState::Idle:
			__noop;
			//animator->SetClip(&monster->clips[uint32_t(Monster::eState::Idle)]);
			break;

		case Monster::eState::Run:
			animator->SetClip(&monster->clips[uint32_t(Monster::eState::Run)]);
			break;

		case Monster::eState::Attack:
			animator->SetClip(&monster->clips[uint32_t(Monster::eState::Attack)]);
			break;

		default:
			assert(false and "지원하지 않는 애니메이션입니다.");
			break;
		}
	}
}

template<uint32_t T>
void MainScene::spawnRangedAttack(const std::array<Entity, T>& entities, const eMonsterType type, const uint32_t spawnFrameIndex)
{
	for (const Entity& monsterEntity : mMonsters)
	{
		Monster* monster = monsterEntity.GetComponent<Monster>();
		if (monster->type != type)
		{
			continue;
		}

		if (const Active* monsterActive = monsterEntity.GetComponent<Active>();
			not monsterActive)
		{
			continue;
		}

		const Animator* monsterAnim = monsterEntity.GetComponent<Animator>();
		if (monsterAnim->clipState != &mArcherClips[uint32_t(Monster::eState::Attack)])
		{
			continue;
		}
			
		if (monsterAnim->frameIndex == spawnFrameIndex)
		{
			for (const Entity& rangeEntity : entities)
			{
				Active* rangeActive = rangeEntity.GetComponent<Active>();
				if (rangeActive->isValue)
				{
					continue;
				}

				RangedAttack* rangedAttack = rangeEntity.GetComponent<RangedAttack>();
				if (rangedAttack->isFire)
				{
					continue;
				}

				if (not monster->isAttackOption)
				{
					monster->isAttackOption = true;
					rangeActive->isValue = true;
					rangedAttack->isFire = true;

					const Point centerOffset = { .x = -0.4f, .y = 0.0f };
					const float centerOffsetX = centerOffset.x * (mArrowTexture.GetWidth() - 1.0f);
					constexpr float monsterLeftOffsetX = 20.0f;
					constexpr float monsterRightOffsetX = 80.0f;

					const Transform* playerTransform = mPlayer.GetComponent<Transform>();
					const Transform* monsterTransform = monsterEntity.GetComponent<Transform>();
					const Point diff = playerTransform->position - monsterTransform->position;

					Direction* rangedDirection = rangeEntity.GetComponent<Direction>();
					rangedDirection->value = Math::NormalizeVector(diff);

					Transform* transform = rangeEntity.GetComponent<Transform>();
					transform->flip = (rangedDirection->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

					float degree = std::atan2(diff.y, diff.x) * (180.0f / 3.141592f);
					degree -= 90.0f;
					transform->angle = -degree;

					// Offset을 계산한다.
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
			}
		}
		else if (monsterAnim->frameIndex >= monsterAnim->clipState->GetLastFrameIndex() - 1)
		{
			Transform* monsterTransform = monsterEntity.GetComponent<Transform>();
			const Transform* playerTransform = mPlayer.GetComponent<Transform>();

			const Point difference = playerTransform->position - monsterTransform->position;
			Direction* direction = monsterEntity.GetComponent<Direction>();
			direction->value = Math::NormalizeVector(difference);
			monsterTransform->flip = (direction->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

			monster->isAttackOption = false;
		}
		else
		{
			monster->isAttackOption = false;
		}
	}
}

template<uint32_t T>
void MainScene::rangedAttackState(const std::array<Entity, T>& entities)
{
	for (const Entity& entity : entities)
	{
		if (Active* active = entity.GetComponent<Active>();
			not active->isValue)
		{
			continue;
		}

		RangedAttack* rangedAttack = entity.GetComponent<RangedAttack>();
		if (not rangedAttack->isFire)
		{
			continue;
		}

		Transform* transform = entity.GetComponent<Transform>();
		if (Math::GetVectorLength(transform->position - rangedAttack->startPosition) >= rangedAttack->distance)
		{
			Active* active = entity.GetComponent<Active>();
			active->isValue = false;

			rangedAttack->isFire = false;
		}
	}
}

template<uint32_t T>
void MainScene::rangedAttackMove(const std::array<Entity, T>& entities, const float speed, const float deltaTime)
{
	for (const Entity& entity : entities)
	{
		Active* active = entity.GetComponent<Active>();
		if (not active->isValue)
		{
			continue;
		}

		Direction* direction = entity.GetComponent<Direction>();
		const Point velocity = direction->value * speed;

		Transform* transform = entity.GetComponent<Transform>();
		transform->position = transform->position + velocity * deltaTime;
	}
}

void MainScene::swordSkillToMonsterCollision()
{
	for (Entity& monsterEntity : mMonsters)
	{
		if (Active* active = monsterEntity.GetComponent<Active>();
			not active->isValue)
		{
			continue;
		}

		Monster::eState monsterState = monsterEntity.GetComponent<Monster>()->state;
		if (monsterState == Monster::eState::Spawn)
		{
			continue;
		}

		if (isCollisionEnter(mSwordSkill, monsterEntity))
		{
			Hp* hp = monsterEntity.GetComponent<Hp>();
			hp->value -= 1;
			
			const Entity& hpBarEntity = mMonsterHpBars[hp->hpBarIndex];

			if (Active* active = hpBarEntity.GetComponent<Active>();
				not active->isValue)
			{
				continue;
			}

			Transform* transform = hpBarEntity.GetComponent<Transform>();
			const float currentWidth = (static_cast<float>(hp->value) / hp->max) * 0.8f;
			transform->scale.width = currentWidth;
		}
		else if (isCollisionStay(mSwordSkill, monsterEntity))
		{
			Active* skillActive = mSwordSkill.GetComponent<Active>();
			skillActive->isValue = false;
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

	screenPosition += mMainCamera.GetComponent<Transform>()->position;

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
	float degree = dgreeOffset + std::atan2(mouseToPlayer.y, mouseToPlayer.x) * (180.0f / 3.141592f);

	transform->position = playerTransform->position;
	transform->flip = playerTransform->flip;

	constexpr Point OFFSET =
	{
		.x = 16.0f,
		.y = 14.0f
	};

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

std::array<Point, 5> MainScene::convertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const
{
	const Point position = transform.position + boxCollider.offset;
	const Scale boxHalfSize = transform.scale * boxCollider.size * 0.5f;

	const Point localTL = { -boxHalfSize.width, -boxHalfSize.height };
	const Point localTR = { boxHalfSize.width, -boxHalfSize.height };
	const Point localBR = { boxHalfSize.width, boxHalfSize.height };
	const Point localBL = { -boxHalfSize.width, boxHalfSize.height };

	const Point rotateTL = Math::RotatePoint(localTL, -transform.angle);
	const Point rotateTR = Math::RotatePoint(localTR, -transform.angle);
	const Point rotateBR = Math::RotatePoint(localBR, -transform.angle);
	const Point rotateBL = Math::RotatePoint(localBL, -transform.angle);

	const std::array<Point, 5> result
	{
		// 왼쪽 위
		Point
		{
			.x = position.x + rotateTL.x,
			.y = position.y - rotateTL.y
		},

		// 오른쪽 위
		Point
		{
			.x = position.x + rotateTR.x,
			.y = position.y - rotateTR.y
		},

		// 오른쪽 아래
		Point
		{
			.x = position.x + rotateBR.x,
			.y = position.y - rotateBR.y
		},

		// 왼쪽 아래
		Point
		{
			.x = position.x + rotateBL.x,
			.y = position.y - rotateBL.y
		},

		// 0번째
		Point
		{
			.x = position.x + rotateTL.x,
			.y = position.y - rotateTL.y
		},
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
	const std::array<Point, 5> point0 = convertBoxColliderToWorldBox(*transform0, *boxCollider0);

	const Transform* transform1 = entity1.GetComponent<Transform>();
	const BoxCollider* boxCollider1 = entity1.GetComponent<BoxCollider>();
	const std::array<Point, 5> point1 = convertBoxColliderToWorldBox(*transform1, *boxCollider1);

	if (Collision::IsCollidedSqureWithSqure(point0, point1))
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

	// TODO: std::array<Point, 5>를 사용해서 수정하기
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

	// TODO: std::array<Point, 5>를 사용해서 수정하기
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