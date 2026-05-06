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

	mBoxTexture.Initialize(GetHelper(), "Resource/box.png");

	for (Entity& mob : mMobs)
	{
		Transform transform{};

		Collider collider(static_cast<uint32_t>(CollisionLayer::Monster));
		collider.CollisionLayerMask.set(uint32_t(CollisionLayer::Monster));

		Image image{};
		image.texture = &mBoxTexture;
		
		Active active{};
		active.value = true;

		Color color{};

		mob.AddComponent(transform);
		mob.AddComponent(collider);
		mob.AddComponent(image);
		mob.AddComponent(color);
		mob.AddComponent(active);

		GetEntityWorld()->AddEntity(&mob);
	}

	{
		Transform* transform = mMobs[1].GetComponent<Transform>();
		transform->position.x += 120.0f;
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

				constexpr float BOX_HALF_SIZE = 16.0f;

				Transform* transform0 = entity0->GetComponent<Transform>();
				Rect rect0 =
				{
					.left = transform0->position.x - BOX_HALF_SIZE,
					.top = transform0->position.y + BOX_HALF_SIZE,
					.right = transform0->position.x + BOX_HALF_SIZE,
					.bottom = transform0->position.y - BOX_HALF_SIZE,
				};

				Transform* transform1 = entity1->GetComponent<Transform>();
				Rect rect1 =
				{
					.left = transform1->position.x - BOX_HALF_SIZE,
					.top = transform1->position.y + BOX_HALF_SIZE,
					.right = transform1->position.x + BOX_HALF_SIZE,
					.bottom = transform1->position.y - BOX_HALF_SIZE,
				};

				if (Collision::IsCollidedSqureWithSqure(rect0, rect1))
				{
					std::pair<const Entity*, const Entity*> collidedEntityPair = getCollidedEntityPair(*entity0, *entity1);

					if (const auto& foundCollidedEntityPair = std::find(mCollidedEntityPairs.begin(), mCollidedEntityPairs.end(), collidedEntityPair);
						foundCollidedEntityPair == mCollidedEntityPairs.end())
					{
						mCollidedEntityPairs.push_back(collidedEntityPair);
					}
				}
			}
		}
	}

	// logic
	{
		{
			const float moveX = float(Input::Get().GetKey(SDL_SCANCODE_D) - Input::Get().GetKey(SDL_SCANCODE_A));

			Transform* transform = mMobs[0].GetComponent<Transform>();
			transform->position.x += moveX * 120.0f * deltaTime;
		}

		if (Input::Get().GetKeyDown(SDL_SCANCODE_SPACE))
		{
			system("cls");
		}

		if (isCollisionEnter(mMobs[0], mMobs[1]))
		{
			printf("충돌 enter\n");
		}
		else if (isCollisionExit(mMobs[0], mMobs[1]))
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

