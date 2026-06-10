#include "pch.h"
#include "StudyScene.h"

#include "Core/Collision.h"
#include "Core/Constant.h"
#include "Core/Input.h"

#include "MainScene.h"

void StudyScene::Initialize()
{
	// Resoruce
	{
		mAliceTexture.Initialize(GetHelper(), "Resource/Char/Alice/Idle/0.png");
		mArrowTexture.Initialize(GetHelper(), "Resource/Monster/Archer/Arrow/0.png");

		// Monseter
		{
			// Spwan
			{
				mspwanTexture.Initialize(GetHelper(), "Resource/Monster/Effect/Die/Die01.png");
				Clip::Frame frame =
				{
					.texture = &mspwanTexture,
					.durationTime = 0.12f
				};
				mArcherClips[uint32_t(Monster::eState::Spawn)].AddClip(frame);
			}

			uint32_t index{};

			// Run
			{
				for (Texture& texture : mArchersRunTextures)
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
				for (Texture& texture : mArchersAttackTextures)
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

	// Camera
	{
		Camera camera{};
		mMainCamera.AddComponent(camera);

		Transform transform{};
		mMainCamera.AddComponent(transform);

		GetEntityWorld()->AddEntity(&mMainCamera);
	}

	 // Monster
	{
		mArcherClips[uint32_t(Monster::eState::Spawn)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		for (Entity& entity : mMonsters)
		{
			Monster monster{};
			monster.state = Monster::eState::Attack;
			entity.AddComponent(monster);

			Transform transform{};
			transform.position.x = 120.0f;
			transform.scale = { .width = 4.0f, .height = 4.0f };
			entity.AddComponent(transform);

			Active active{};
			active.isValue = true;
			entity.AddComponent(active);

			Image image{};
			entity.AddComponent(image);

			Animator anim{};
			anim.clipState = &mArcherClips[0];
			entity.AddComponent(anim);

			Color color{};
			entity.AddComponent(color);

			//CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Monster));
			//collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Player));
			//entity.AddComponent(collider);

			BoxCollider boxCollider{};
			boxCollider.size = { .width = float(mAliceTexture.GetWidth()), .height = float(mAliceTexture.GetHeight()) };
			entity.AddComponent(boxCollider);

			DebugActive debugActive{};
			debugActive.isValue = true;
			entity.AddComponent(debugActive);

			DebugColor debugColor{};
			entity.AddComponent(debugColor);

			GetEntityWorld()->AddEntity(&entity);
		}
	}

	// Player
	{
		Transform transform{};
		transform.position = { .x = -200.0f, .y = 0.0f };
		transform.scale = { .width = 4.0f, .height = 4.0f };
		mPlayer.AddComponent(transform);

		Image image{};
		image.texture = &mAliceTexture;
		mPlayer.AddComponent(image);

		Active active{};
		active.isValue = true;
		mPlayer.AddComponent(active);

		Color color{};
		mPlayer.AddComponent(color);

		CollisionDetector collider(static_cast<uint32_t>(MainScene::CollisionLayer::Player));
		collider.CollisionLayerMask.set(uint32_t(MainScene::CollisionLayer::Monster));
		mPlayer.AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.size = { .width = float(mAliceTexture.GetWidth()), .height = float(mAliceTexture.GetHeight()) };
		mPlayer.AddComponent(boxCollider);

		DebugActive debugActive{};
		debugActive.isValue = true;
		mPlayer.AddComponent(debugActive);

		DebugColor debugColor{};
		mPlayer.AddComponent(debugColor);

		GetEntityWorld()->AddEntity(&mPlayer);
	}

	// Arrow
	{
		for (Entity& entity : mArrows)
		{
			Transform transform{};
			transform.scale = { .width = 4.0f, .height = 4.0f };
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
}

bool StudyScene::Update(const float deltaTime)
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

	// Camera
	{
		//Transform* transform = mMainCamera.GetComponent<Transform>();
		//Transform* target = mPlayer.GetComponent<Transform>();
		//transform->position = target->position;
	}

	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
	{
		return false;
	}
	if (Input::Get().GetKeyDown(SDL_SCANCODE_SPACE))
	{
		system("cls");
	}
	
	// Player Move
	{
		const float moveX = float(Input::Get().GetKey(SDL_SCANCODE_D) - Input::Get().GetKey(SDL_SCANCODE_A));
		const float moveY = float(Input::Get().GetKey(SDL_SCANCODE_W) - Input::Get().GetKey(SDL_SCANCODE_S));

		Transform* transform = mPlayer.GetComponent<Transform>();
		transform->position.x += moveX * 500.0f * deltaTime;
		transform->position.y += moveY * 500.0f * deltaTime;
	}

	// Remove Monster
	{
		static float timer;
		timer += deltaTime;

		if (timer >= 3.0f)
		{
			mMonsters[0].SetRemoved(true);
		}
	}

	// Arrow Active
	{
		static float fireTimer[3];
		static Point startPosition[3];
		static bool isFire;

		Animator* anim = mMonsters[0].GetComponent<Animator>();
		if (anim->clipState == &mArcherClips[uint32_t(Monster::eState::Attack)]
			and anim->frameIndex == 7)
		{
			if (not isFire)
			{
				for (uint32_t i = 0; i < mArrows.size(); ++i)
				{
					Entity& entity = mArrows[i];
					Active* active = entity.GetComponent<Active>();
					if (not active->isValue)
					{
						const Point centerOffset = { .x = -0.4f, .y = 0.0f };
						const float centerOffsetX = centerOffset.x * (mArrowTexture.GetWidth() - 1.0f);
						constexpr float monsterOffsetX = 80.0f;

						Transform* monsterTransform = mMonsters[0].GetComponent<Transform>();
						startPosition[i].x = monsterTransform->position.x 
							+ (centerOffsetX - mArrowTexture.GetWidth()) * mArrowTexture.GetWidth() + monsterOffsetX;
						startPosition[i].y = monsterTransform->position.y;

						Transform* transform = entity.GetComponent<Transform>();
						transform->position.x = monsterTransform->position.x 
							+ (centerOffsetX - mArrowTexture.GetWidth()) * mArrowTexture.GetWidth() + monsterOffsetX;
						transform->position.y = monsterTransform->position.y;

						Direction* direction = entity.GetComponent<Direction>();
						Transform* playerTransform = mPlayer.GetComponent<Transform>();
						const Point diff = playerTransform->position - monsterTransform->position;
						direction->value = Math::NormalizeVector(diff);
						transform->flip = (direction->value.x > 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

						float degree = std::atan2(diff.y, diff.x) * (180.0f / 3.141592f);
						degree -= 90.0f;
						transform->angle = -degree;

						active->isValue = true;
						isFire = true;

						DebugActive* debugActive = entity.GetComponent<DebugActive>();
						debugActive->isValue = true;
						break;
					}
				}
			}
		}
		else
		{
			isFire = false;
		}

		for (uint32_t i = 0; i < mArrows.size(); ++i)
		{
			Entity& entity = mArrows[i];
			Active* active = entity.GetComponent<Active>();
			if (not active->isValue)
			{
				continue;
			}

			if (anim->clipState == &mArcherClips[uint32_t(Monster::eState::Attack)]
				and anim->frameIndex == anim->clipState->GetLastFrameIndex() - 1)
			{
				anim->frameIndex = anim->clipState->GetLastFrameIndex() - 1;
				anim->elapsedTime = 0.0f;

				fireTimer[i] += deltaTime;
				if (fireTimer[i] >= 0.3f)
				{
					isFire = false;
					anim->frameIndex = 0;
					fireTimer[i] = 0.0f;
				}
			}

			Transform* transform = entity.GetComponent<Transform>();
			if (Math::GetVectorLength(transform->position - startPosition[i]) >= 200.0f)
			{
				Active* active = entity.GetComponent<Active>();
				active->isValue = false;

				DebugActive* debugActive = entity.GetComponent<DebugActive>();
				debugActive->isValue = false;
			}
		}
	}

	Active* active = mMonsters[0].GetComponent<Active>();

	// Arrow Move
	{
		for (Entity& entity : mArrows)
		{
			Active* active = entity.GetComponent<Active>();
			if (not active->isValue)
			{
				continue;
			}

			Direction* direction = entity.GetComponent<Direction>();
			const Point velocity = direction->value * 100.0f;

			Transform* transform = entity.GetComponent<Transform>();
			transform->position = transform->position + velocity * deltaTime;
		}
	}

	// 충돌 업데이트한다.
	{
		//for (const auto& monster : mMonsters)
		//{
		//	if (isCollisionEnter(mPlayer, monster))
		//	{
		//		//if (monster.GetComponent<Monster>()->state == Monster::eState::Run)
		//		{
		//			printf("몬 - 플 충돌\n");
		//		}
		//	}
		//	else if (isCollisionStay(mPlayer, monster))
		//	{
		//		//if (monster.GetComponent<Monster>()->state == Monster::eState::Attack)
		//		{
		//		}
		//	}
		//}

		for (const auto& arrow : mArrows)
		{
			if (not arrow.GetComponent<Active>()->isValue)
			{
				continue;
			}

			if (isCollisionEnter(mPlayer, arrow))
			{
				printf("플 - 화 충돌\n");
			}
			else if (isCollisionStay(mPlayer, arrow))
			{
			}
		}

		mPreviousCollidedEntityPairs = mCollidedEntityPairs;
		mCollidedEntityPairs.clear();
	}

	for (Entity& entity : mMonsters)
	{
		Animator* animator = entity.GetComponent<Animator>();
		Monster* monster = entity.GetComponent<Monster>();

		switch (monster->state)
		{
		case Monster::eState::Spawn:
			animator->SetClip(&mArcherClips[uint32_t(Monster::eState::Spawn)]);
			break;

		case Monster::eState::Run:
			animator->SetClip(&mArcherClips[uint32_t(Monster::eState::Run)]);
			break;

		case Monster::eState::Attack:
			animator->SetClip(&mArcherClips[uint32_t(Monster::eState::Attack)]);
			break;

		default:
			assert(false and "지원하지 않는 애니메이션입니다.");
			break;
		}
	}

	return true;
}

void StudyScene::Finalize()
{
	mAliceTexture.Finalize();
	mspwanTexture.Finalize();
	mArrowTexture.Finalize();

	for (Texture& texture : mArchersIdleTextures)
	{
		texture.Finalize();
	}

	for (Texture& texture : mArchersRunTextures)
	{
		texture.Finalize();
	}

	for (Texture& texture : mArchersAttackTextures)
	{
		texture.Finalize();
	}
}

Point StudyScene::getScreenMousePosition() const
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

bool StudyScene::checkCollisionBoxBox(const Entity& entity0, const Entity& entity1)
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

Quad StudyScene::convertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const
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
		.rightTop = {.x = position.x + rotate.rightTop.x, .y = position.y - rotate.rightTop.y },
		.leftBottom = {.x = position.x + rotate.leftBottom.x, .y = position.y - rotate.leftBottom.y },
		.rightBottom = {.x = position.x + rotate.rightBottom.x, .y = position.y - rotate.rightBottom.y }
	};

	return result;
}

void StudyScene::registerCollidedEntityPairs(const Entity& entity0, const Entity& entity1)
{
	std::pair<const Entity*, const Entity*> colliderEntityPair = getCollidedEntityPair(entity0, entity1);

	if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), colliderEntityPair);
		foundCollidedEntityPair == mCollidedEntityPairs.end())
	{
		mCollidedEntityPairs.push_back(colliderEntityPair);
	}
}

std::pair<const Entity*, const Entity*> StudyScene::getCollidedEntityPair(const Entity& entity0, const Entity& entity1) const
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

bool StudyScene::isCollisionEnter(const Entity& entity0, const Entity& entity1) const
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

bool StudyScene::isCollisionStay(const Entity& entity0, const Entity& entity1) const
{
	std::pair<const Entity*, const Entity*> collidedEntityPair = getCollidedEntityPair(entity0, entity1);

	const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
	return foundCollidedEntityPair != mCollidedEntityPairs.end();
}

bool StudyScene::isCollisionExit(const Entity& entity0, const Entity& entity1) const
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