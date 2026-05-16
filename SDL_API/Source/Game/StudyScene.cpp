#include "pch.h"
#include "StudyScene.h"

#include "Core/Collision.h"
#include "Core/Constant.h"
#include "Core/Input.h"

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
				mArcherClips[uint32_t(Monster::eState::Spwan)].AddClip(frame);
			}

			uint32_t index{};
			// Idle
			{
				for (Texture& texture : mArchersIdleTextures)
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
		mArcherClips[uint32_t(Monster::eState::Spwan)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Idle)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Run)].SetLoop(true);
		mArcherClips[uint32_t(Monster::eState::Attack)].SetLoop(true);

		for (Entity& entity : mArchers)
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

			Animator anim{};
			anim.clipState = &mArcherClips[0];
			entity.AddComponent(anim);

			Color color{};
			entity.AddComponent(color);

			GetEntityWorld()->AddEntity(&entity);
		}
	}


	// Player
	{
		Transform transform{};
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

		GetEntityWorld()->AddEntity(&mPlayer);
	}

	// Arrow
	{
		uint32_t index{};
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

			Color color{};
			entity.AddComponent(color);

			GetEntityWorld()->AddEntity(&entity);
		}
		index = 0;
	}
}

bool StudyScene::Update(const float deltaTime)
{
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

	// Arrow Active
	{
		static float fireTimer[3];
		static Point startPosition[3];
		static bool isFire;

		Animator* anim = mArchers[0].GetComponent<Animator>();
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

						Transform* monsterTransform = mArchers[0].GetComponent<Transform>();
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
				if (fireTimer[i] >= 0.7f)
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
			}
		}
	}

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

	for (Entity& entity : mArchers)
	{
		Animator* animator = entity.GetComponent<Animator>();
		Monster* monster = entity.GetComponent<Monster>();

		switch (monster->state)
		{
		case Monster::eState::None:
			__noop;
			break;

		case Monster::eState::Spwan:
			animator->SetClip(&mArcherClips[uint32_t(Monster::eState::Spwan)]);
			break;

		case Monster::eState::Idle:
			animator->SetClip(&mArcherClips[uint32_t(Monster::eState::Idle)]);
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
