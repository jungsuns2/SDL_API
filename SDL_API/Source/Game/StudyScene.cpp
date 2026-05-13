#include "pch.h"
#include "StudyScene.h"
//
//#include "Core/Input.h"
//#include "Core/Collision.h"
//
//void StudyScene::Initialize()
//{
//	 Resource
//	mBoxTexture.Initialize(GetHelper(), "Resource/RedRectangle.png");
//	mCircleTexture.Initialize(GetHelper(), "Resource/RedCircle.png");
//
//	 Camera
//	{
//		Camera camera{};
//		mMainCamera.AddComponent(camera);
//
//		Transform transform{};
//		mMainCamera.AddComponent(transform);
//
//		GetEntityWorld()->AddEntity(&mMainCamera);
//	}
//
//	 Monster
//	{
//		Transform transform{};
//		transform.position.x = 120.0f;
//		transform.scale.width = 2.0f;
//		mRectMonster.AddComponent(transform);
//
//		Image image{};
//		image.texture = &mBoxTexture;
//		mRectMonster.AddComponent(image);
//
//		Active active{};
//		active.value = true;
//		mRectMonster.AddComponent(active);
//
//		Color color{};
//		mRectMonster.AddComponent(color);
//
//		CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
//		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
//		mRectMonster.AddComponent(collider);
//
//		BoxCollider boxCollider{};
//		boxCollider.size = { .width = float(mBoxTexture.GetWidth()), .height = float(mBoxTexture.GetHeight()) };
//		mRectMonster.AddComponent(boxCollider);
//
//		GetEntityWorld()->AddEntity(&mRectMonster);
//	}
//
//	 Circle Monster
//	{
//		Transform transform{};
//		transform.position = { .x = 120.0f, .y = 120.0f };
//		transform.scale = { .width = 2.0f, .height = 2.0f }; 
//		mCircleMonster.AddComponent(transform);
//
//		Image image{};
//		image.texture = &mCircleTexture;
//		mCircleMonster.AddComponent(image);
//
//		Active active{};
//		active.value = true;
//		mCircleMonster.AddComponent(active);
//
//		Color color{};
//		mCircleMonster.AddComponent(color);
//
//		CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
//		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
//		mCircleMonster.AddComponent(collider);
//
//		CircleCollider circleCollider{};
//		circleCollider.radius = mCircleTexture.GetWidth() * 0.5f;
//		mCircleMonster.AddComponent(circleCollider);
//
//		GetEntityWorld()->AddEntity(&mCircleMonster);
//	}
//
//	 Line Monster
//	{
//		Transform transform{};
//		transform.position = { .x = 120.0f, .y = -240.0f };
//		transform.scale = { .width = 3.0f, .height = 0.1f };
//		mLineMonster.AddComponent(transform);
//
//		Image image{};
//		image.texture = &mBoxTexture;
//		mLineMonster.AddComponent(image);
//
//		Active active{};
//		active.value = true;
//		mLineMonster.AddComponent(active);
//
//		Color color{};
//		mLineMonster.AddComponent(color);
//
//		CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
//		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
//		mLineMonster.AddComponent(collider);
//
//		LineCollider lineCollider{};
//		lineCollider.scale = { .width = float(mBoxTexture.GetWidth()), .height = float(mBoxTexture.GetHeight()) };
//		mLineMonster.AddComponent(lineCollider);
//
//		GetEntityWorld()->AddEntity(&mLineMonster);
//	}
//
//	 Player
//	{
//		Transform transform{};
//		mRectPlayer.AddComponent(transform);
//
//		Image image{};
//		image.texture = &mBoxTexture;
//		mRectPlayer.AddComponent(image);
//
//		Active active{};
//		active.value = true;
//		mRectPlayer.AddComponent(active);
//
//		Color color{};
//		mRectPlayer.AddComponent(color);
//
//		CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
//		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
//		mRectPlayer.AddComponent(collider);
//
//		BoxCollider boxCollider{};
//		boxCollider.size = { .width = float(mBoxTexture.GetWidth()), .height = float(mBoxTexture.GetHeight()) };
//		mRectPlayer.AddComponent(boxCollider);
//
//		GetEntityWorld()->AddEntity(&mRectPlayer);
//	}
//
//	 Circle Player
//	{
//		Transform transform{};
//		mCirclePlayer.AddComponent(transform);
//
//		Image image{};
//		image.texture = &mCircleTexture;
//		mCirclePlayer.AddComponent(image);
//
//		Active active{};
//		active.value = true;
//		mCirclePlayer.AddComponent(active);
//
//		Color color{};
//		mCirclePlayer.AddComponent(color);
//
//		CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
//		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
//		mCirclePlayer.AddComponent(collider);
//
//		CircleCollider circleCollider{};
//		circleCollider.radius = mCircleTexture.GetWidth() * 0.5f;
//		mCirclePlayer.AddComponent(circleCollider);
//
//		GetEntityWorld()->AddEntity(&mCirclePlayer);
//	}
//}
//
//bool StudyScene::Update(const float deltaTime)
//{
//	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
//	{
//		return false;
//	}
//
//	 Core
//	{
//		for (const Entity* entity0 : GetEntityWorld()->GetAllEntites())
//		{
//			if (not entity0->HasComponent<Transform>()
//				or not entity0->HasComponent<CollisionDetector>())
//			{
//				continue;
//			}
//
//			CollisionDetector* collisionDetector0 = entity0->GetComponent<CollisionDetector>();
//			if (collisionDetector0->CollisionLayerMask.none())
//			{
//				continue;
//			}
//
//			for (const Entity* entity1 : GetEntityWorld()->GetAllEntites())
//			{
//				if (not entity1->HasComponent<Transform>()
//					or not entity1->HasComponent<CollisionDetector>())
//				{
//					continue;
//				}
//
//				if (entity0 == entity1)
//				{
//					continue;
//				}
//
//				CollisionDetector* collisionDetector1 = entity1->GetComponent<CollisionDetector>();
//				if (not collisionDetector0->CollisionLayerMask[collisionDetector1->Layer])
//				{
//					continue;
//				}
//
//				{
//					checkCollisionBoxBox(*entity0, *entity1) 
//						or checkCollisionBoxCircle(*entity0, *entity1)
//						or checkCollisionBoxLine(*entity0, *entity1)
//						or checkCollisionCircleCircle(*entity0, *entity1)
//						or checkCollisionCircleLine(*entity0, *entity1);
//				}
//			}
//		}
//	}
//
//	 logic
//	{
//		{
//			const float moveX = float(Input::Get().GetKey(SDL_SCANCODE_D) - Input::Get().GetKey(SDL_SCANCODE_A));
//			const float moveY = float(Input::Get().GetKey(SDL_SCANCODE_W) - Input::Get().GetKey(SDL_SCANCODE_S));
//
//			Transform* transform = mCirclePlayer.GetComponent<Transform>();
//			transform->position.x += moveX * 120.0f * deltaTime;
//			transform->position.y += moveY * 120.0f * deltaTime;
//		}
//
//		if (Input::Get().GetKeyDown(SDL_SCANCODE_SPACE))
//		{
//			system("cls");
//		}
//
//		if (isCollisionEnter(mRectPlayer, mRectMonster))
//		{
//			printf("面倒 enter\n");
//		}
//		else if (isCollisionExit(mRectPlayer, mRectMonster))
//		{
//			printf("面倒 exit\n");
//		}
//
//		if (isCollisionEnter(mRectPlayer, mCircleMonster))
//		{
//			printf("面倒 enter\n");
//		}
//		else if (isCollisionExit(mRectPlayer, mCircleMonster))
//		{
//			printf("面倒 exit\n");
//		}
//
//		if (isCollisionEnter(mRectPlayer, mLineMonster))
//		{
//			printf("面倒 enter\n");
//		}
//		else if (isCollisionExit(mRectPlayer, mLineMonster))
//		{
//			printf("面倒 exit\n");
//		}
//
//		if (isCollisionEnter(mCirclePlayer, mCircleMonster))
//		{
//			printf("面倒 enter\n");
//		}
//		else if (isCollisionExit(mCirclePlayer, mCircleMonster))
//		{
//			printf("面倒 exit\n");
//		}
//
//		if (isCollisionEnter(mCirclePlayer, mLineMonster))
//		{
//			printf("面倒 enter\n");
//		}
//		else if (isCollisionExit(mCirclePlayer, mLineMonster))
//		{
//			printf("面倒 exit\n");
//		}
//	}
//
//	{
//		mPreviousCollidedEntityPairs = mCollidedEntityPairs;
//		mCollidedEntityPairs.clear();
//	}
//
//	return true;
//}
//
//void StudyScene::Finalize()
//{
//	mBoxTexture.Finalize();
//	mCircleTexture.Finalize();
//}
//
//bool StudyScene::isCollisionEnter(const Entity& entity0, const Entity& entity1) const
//{
//	std::pair<const Entity*, const Entity*> collidedEntityPair = getCollidedEntityPair(entity0, entity1);
//
//	if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
//		foundCollidedEntityPair != mCollidedEntityPairs.end())
//	{
//		const auto& foundPreviousCollidedEntityPair = std::find(mPreviousCollidedEntityPairs.begin(), mPreviousCollidedEntityPairs.end(), collidedEntityPair);
//		return foundPreviousCollidedEntityPair == mPreviousCollidedEntityPairs.end();
//	}
//
//	return false;
//}
//
//bool StudyScene::isCollisionStay(const Entity& entity0, const Entity& entity1) const
//{
//	std::pair<const Entity*, const Entity*> collidedEntityPair = getCollidedEntityPair(entity0, entity1);
//
//	const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
//	return foundCollidedEntityPair != mCollidedEntityPairs.end();
//}
//
//bool StudyScene::isCollisionExit(const Entity& entity0, const Entity& entity1) const
//{
//	std::pair<const Entity*, const Entity*> collidedEntityPair = getCollidedEntityPair(entity0, entity1);
//
//	if (const auto& foundPreviousCollidedEntityPair = std::find(mPreviousCollidedEntityPairs.begin(), mPreviousCollidedEntityPairs.end(), collidedEntityPair);
//		foundPreviousCollidedEntityPair != mPreviousCollidedEntityPairs.end())
//	{
//		const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
//		return foundCollidedEntityPair == mCollidedEntityPairs.end();
//	}
//
//	return false;
//}
//
//std::pair<const Entity*, const Entity*> StudyScene::getCollidedEntityPair(const Entity& entity0, const Entity& entity1) const
//{
//	std::pair<const Entity*, const Entity*> collidedEntityPair{};
//	if (&entity0 < &entity1)
//	{
//		collidedEntityPair = { &entity0, &entity1 };
//	}
//	else
//	{
//		collidedEntityPair = { &entity1, &entity0 };
//	}
//
//	return collidedEntityPair;
//}
//
//void StudyScene::registerCollidedEntityPairs(const Entity& entity0, const Entity& entity1)
//{
//	std::pair<const Entity*, const Entity*> colliderEntityPair = getCollidedEntityPair(entity0, entity1);
//
//	if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), colliderEntityPair);
//		foundCollidedEntityPair == mCollidedEntityPairs.end())
//	{
//		mCollidedEntityPairs.push_back(colliderEntityPair);
//	}
//}
//
//Rect StudyScene::convertBoxColliderToWorldBox(const Transform& transform, const BoxCollider& boxCollider) const
//{
//	const Point position = transform.position + boxCollider.offset;
//	const Scale boxHalfSize = transform.scale * boxCollider.size * 0.5f;
//
//	const Rect result
//	{
//		.left = position.x - boxHalfSize.width,
//		.top = position.y + boxHalfSize.height,
//		.right = position.x + boxHalfSize.width,
//		.bottom = position.y - boxHalfSize.height,
//	};
//
//	return result;
//}
//
//Circle StudyScene::convertCircleColliderToWorldCircle(const Transform& transform, const CircleCollider& circleCollider) const
//{
//	const Circle result =
//	{
//		.center = transform.position + circleCollider.offset,
//		.radius = transform.scale.width * circleCollider.radius
//	};
//	
//	return result;
//}
//
//Line StudyScene::convertLineColliderToWorldLine(const Transform& transform, const LineCollider& lineCollider) const
//{
//	const Point position = transform.position + lineCollider.offset;
//	const Scale boxHalfSize = transform.scale * lineCollider.scale * 0.5f;
//
//	const Rect rect
//	{
//		.left = position.x - boxHalfSize.width,
//		.top = position.y + boxHalfSize.height,
//		.right = position.x + boxHalfSize.width,
//		.bottom = position.y - boxHalfSize.height,
//	};
//
//	float centerY = rect.top + (transform.scale.height * lineCollider.scale.height * 0.5f);
//
//	const Line result =
//	{
//		.point0 = { .x = rect.left, .y = rect.top },
//		.point1 = { .x = rect.right, .y = rect.bottom }
//	};
//
//	return result;
//}
//
//bool StudyScene::checkCollisionBoxBox(const Entity& entity0, const Entity& entity1)
//{
//	if (not entity0.HasComponent<BoxCollider>()
//		or not entity1.HasComponent<BoxCollider>())
//	{
//		return false;
//	}
//
//	const Transform* transform0 = entity0.GetComponent<Transform>();
//	const BoxCollider* boxCollider0 = entity0.GetComponent<BoxCollider>();
//	const Rect rect0 = convertBoxColliderToWorldBox(*transform0, *boxCollider0);
//
//	const Transform* transform1 = entity1.GetComponent<Transform>();
//	const BoxCollider* boxCollider1 = entity1.GetComponent<BoxCollider>();
//	const Rect rect1 = convertBoxColliderToWorldBox(*transform1, *boxCollider1);
//
//	if (Collision::IsCollidedSqureWithSqure(rect0, rect1))
//	{
//		registerCollidedEntityPairs(entity0, entity1);
//		return true;
//	}
//
//	return false;
//}
//
//bool StudyScene::checkCollisionBoxCircle(const Entity& boxEntity, const Entity& circleEntity)
//{
//	if (not boxEntity.HasComponent<BoxCollider>()
//		or not circleEntity.HasComponent<CircleCollider>())
//	{
//		return false;
//	}
//
//	const Transform* boxTransform = boxEntity.GetComponent<Transform>();
//	const BoxCollider* boxCollider = boxEntity.GetComponent<BoxCollider>();
//	const Rect rect = convertBoxColliderToWorldBox(*boxTransform, *boxCollider);
//
//	const Transform* circleTransform = circleEntity.GetComponent<Transform>();
//	const CircleCollider* circleCollider = circleEntity.GetComponent<CircleCollider>();
//	const Circle circle = convertCircleColliderToWorldCircle(*circleTransform, *circleCollider);
//
//	if (Collision::IsCollidedSqureWithCircle(rect, circle))
//	{
//		registerCollidedEntityPairs(boxEntity, circleEntity);
//		return true;
//	}
//
//	return false;
//}
//
//bool StudyScene::checkCollisionBoxLine(const Entity& boxEntity, const Entity& lineEntity)
//{
//	if (not boxEntity.HasComponent<BoxCollider>()
//		or not lineEntity.HasComponent<LineCollider>())
//	{
//		return false;
//	}
//
//	const Transform* boxTransform = boxEntity.GetComponent<Transform>();
//	const BoxCollider* boxCollider = boxEntity.GetComponent<BoxCollider>();
//	const Rect rect = convertBoxColliderToWorldBox(*boxTransform, *boxCollider);
//
//	const Transform* lineTransform = lineEntity.GetComponent<Transform>();
//	const LineCollider* lineCollider = lineEntity.GetComponent<LineCollider>();
//	const Line line = convertLineColliderToWorldLine(*lineTransform, *lineCollider);
//
//	if (Collision::IsCollidedSqureWithLine(rect, line))
//	{
//		registerCollidedEntityPairs(boxEntity, lineEntity);
//		return true;
//	}
//
//	return false;
//}
//
//bool StudyScene::checkCollisionCircleCircle(const Entity& entity0, const Entity& entity1)
//{
//	if (not entity0.HasComponent<CircleCollider>()
//		or not entity1.HasComponent<CircleCollider>())
//	{
//		return false;
//	}
//
//	const Transform* Transform0 = entity0.GetComponent<Transform>();
//	const CircleCollider* Collider0 = entity0.GetComponent<CircleCollider>();
//	const Circle circle0 = convertCircleColliderToWorldCircle(*Transform0, *Collider0);
//
//	const Transform* Transform1 = entity1.GetComponent<Transform>();
//	const CircleCollider* Collider1 = entity1.GetComponent<CircleCollider>();
//	const Circle circle1 = convertCircleColliderToWorldCircle(*Transform1, *Collider1);
//
//	if (Collision::IsCollidedCircleWithCircle(circle0, circle1))
//	{
//		registerCollidedEntityPairs(entity0, entity1);
//		return true;
//	}
//
//	return false;
//}
//
//bool StudyScene::checkCollisionCircleLine(const Entity& circleEntity, const Entity& lineEntity)
//{
//	if (not circleEntity.HasComponent<CircleCollider>()
//		or not lineEntity.HasComponent<LineCollider>())
//	{
//		return false;
//	}
//
//	const Transform* circleTransform = circleEntity.GetComponent<Transform>();
//	const CircleCollider* circleCollider = circleEntity.GetComponent<CircleCollider>();
//	const Circle circle = convertCircleColliderToWorldCircle(*circleTransform, *circleCollider);
//
//	const Transform* lineTransform = lineEntity.GetComponent<Transform>();
//	const LineCollider* lineCollider = lineEntity.GetComponent<LineCollider>();
//	const Line line = convertLineColliderToWorldLine(*lineTransform, *lineCollider);
//
//	if (Collision::IsCollidedCircleWithLine(circle, line))
//	{
//		registerCollidedEntityPairs(circleEntity, lineEntity);
//		return true;
//	}
//
//	return false;
//}