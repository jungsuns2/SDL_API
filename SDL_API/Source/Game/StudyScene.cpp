#include "pch.h"
#include "StudyScene.h"

#include "Core/Input.h"
#include "Core/Collision.h"

enum class CollisionShape
{
	None,
	Box,
	Line,
	Circle,
	Point,
	Count
};

// Game
enum class CollisionLayer
{
	Player,
	Monster,
	Wall,
	Count
};

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

void StudyScene::Initialize()
{
	// Resource
	mBoxTexture.Initialize(GetHelper(), "Resource/RedRectangle.png");
	mCircleTexture.Initialize(GetHelper(), "Resource/RedCircle.png");

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
		Transform transform{};
		transform.position.x = 120.0f;
		mRectMonster.AddComponent(transform);

		Image image{};
		image.texture = &mBoxTexture;
		mRectMonster.AddComponent(image);

		Active active{};
		active.value = true;
		mRectMonster.AddComponent(active);

		Color color{};
		mRectMonster.AddComponent(color);

		CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
		mRectMonster.AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.size = { .width = float(mBoxTexture.GetWidth()), .height = float(mBoxTexture.GetHeight()) };
		mRectMonster.AddComponent(boxCollider);

		GetEntityWorld()->AddEntity(&mRectMonster);
	}

	// Player
	{
		Transform transform{};
		mPlayer.AddComponent(transform);

		Image image{};
		image.texture = &mBoxTexture;
		mPlayer.AddComponent(image);

		Active active{};
		active.value = true;
		mPlayer.AddComponent(active);

		Color color{};
		mPlayer.AddComponent(color);

		CollisionDetector collider(static_cast<uint32_t>(CollisionLayer::Monster));
		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));
		mPlayer.AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.size = { .width = float(mBoxTexture.GetWidth()), .height = float(mBoxTexture.GetHeight()) };
		mPlayer.AddComponent(boxCollider);

		GetEntityWorld()->AddEntity(&mPlayer);
	}
}

bool StudyScene::Update(const float deltaTime)
{
	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
	{
		return false;
	}

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
					checkCollisionBoxBox(*entity0, *entity1);
				}
			}
		}
	}

	// logic
	{
		{
			const float moveX = float(Input::Get().GetKey(SDL_SCANCODE_D) - Input::Get().GetKey(SDL_SCANCODE_A));
			const float moveY = float(Input::Get().GetKey(SDL_SCANCODE_W) - Input::Get().GetKey(SDL_SCANCODE_S));

			Transform* transform = mPlayer.GetComponent<Transform>();
			transform->position.x += moveX * 120.0f * deltaTime;
			transform->position.y += moveY * 120.0f * deltaTime;
		}

		if (Input::Get().GetKeyDown(SDL_SCANCODE_SPACE))
		{
			system("cls");
		}

		if (isCollisionEnter(mPlayer, mRectMonster))
		{
			printf("충돌 enter\n");
		}
		else if (isCollisionExit(mPlayer, mRectMonster))
		{
			printf("충돌 exit\n");
		}
	}

	{
		mPreviousCollidedEntityPairs = mCollidedEntityPairs;
		mCollidedEntityPairs.clear();
	}

	return true;
}

void StudyScene::Finalize()
{
	mBoxTexture.Finalize();
	mCircleTexture.Finalize();
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

Rect StudyScene::convertBoxColliderToWorldRect(const Transform& transform, const BoxCollider& boxCollider) const
{
	const Point position = transform.position + boxCollider.offset;
	const Scale boxHalfSize = boxCollider.size * 0.5f;

	const Rect result
	{
		.left = position.x - boxHalfSize.width,
		.top = position.y + boxHalfSize.height,
		.right = position.x + boxHalfSize.width,
		.bottom = position.y - boxHalfSize.height,
	};

	return result;
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

void StudyScene::registerCollidedEntityPairs(const Entity& entity0, const Entity& entity1)
{
	std::pair<const Entity*, const Entity*> colliderEntityPair = getCollidedEntityPair(entity0, entity1);

	if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), colliderEntityPair);
		foundCollidedEntityPair == mCollidedEntityPairs.end())
	{
		mCollidedEntityPairs.push_back(colliderEntityPair);
	}
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
	const Rect rect0 = convertBoxColliderToWorldRect(*transform0, *boxCollider0);

	const Transform* transform1 = entity1.GetComponent<Transform>();
	const BoxCollider* boxCollider1 = entity1.GetComponent<BoxCollider>();
	const Rect rect1 = convertBoxColliderToWorldRect(*transform1, *boxCollider1);

	if (Collision::IsCollidedSqureWithSqure(rect0, rect1))
	{
		registerCollidedEntityPairs(entity0, entity1);
		return true;
	}

	return false;
}