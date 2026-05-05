#include "pch.h"
#include "StudyScene.h"

#include "Core/Input.h"

// Game
enum class CollisionLayer
{
	Player,
	Monster,
	Wall,
	Count
};

// Core
struct Collider final : public Component
{
	static constexpr uint32_t _ID = 0;
	Collider(const uint32_t layer) : Component(&_ID), Layer(layer) {}

	const uint32_t Layer = 0;
	std::bitset<64> CollisionLayerMask{};
};

void StudyScene::Initialize()
{
	// Camera
	{
		Camera camera{};
		mMainCamera.AddComponent(camera);

		Transform transform{};
		mMainCamera.AddComponent(transform);

		GetEntityWorld()->AddEntity(&mMainCamera);
	}

	for (Entity& mob : mMobs)
	{
		Transform transform{};
		Collider collider(static_cast<uint32_t>(CollisionLayer::Monster));
		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Player));
		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Wall));

		mob.AddComponent(transform);
		mob.AddComponent(collider);

		GetEntityWorld()->AddEntity(&mob);
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
				or not entity0->HasComponent<Collider>())
			{
				continue;
			}

			Collider* collider0 = entity0->GetComponent<Collider>();
			if (collider0->CollisionLayerMask.none())
			{
				continue;
			}

			for (const Entity* entity1 : GetEntityWorld()->GetAllEntites())
			{
				if (not entity1->HasComponent<Transform>()
					or not entity1->HasComponent<Collider>())
				{
					continue;
				}

				if (entity0 == entity1)
				{
					continue;
				}

				Collider* collider1 = entity1->GetComponent<Collider>();
				if (not collider0->CollisionLayerMask[collider1->Layer])
				{
					continue;
				}

				if (1 /* 충돌했나? */)
				{
					std::pair<const Entity*, const Entity*> collidedEntityPair{};
					if (entity0 < entity1)
					{
						collidedEntityPair = { entity0, entity1 };
					}
					else
					{
						collidedEntityPair = { entity1, entity0 };
					}

					if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
						foundCollidedEntityPair == mCollidedEntityPairs.end())
					{
						mCollidedEntityPairs.push_back(collidedEntityPair);
					}
				}
			}
		}

		for (const auto& collidedEntityPair : mCollidedEntityPairs)
		{
			if (const auto& foundCollidedEntityPair = std::find(mPreviousCollidedEntityPairs.begin(), mPreviousCollidedEntityPairs.end(), collidedEntityPair);
				foundCollidedEntityPair == mPreviousCollidedEntityPairs.end())
			{
				// enter
			}
			else
			{
				// stay
			}
		}

		for (const auto& collidedEntityPair : mPreviousCollidedEntityPairs)
		{
			if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
				foundCollidedEntityPair == mCollidedEntityPairs.end())
			{
				// exit
			}
		}

		mPreviousCollidedEntityPairs = mCollidedEntityPairs;
		mCollidedEntityPairs.clear();
	}

	return true;
}

void StudyScene::Finalize()
{
}
