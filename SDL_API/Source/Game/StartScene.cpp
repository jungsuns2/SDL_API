#include "pch.h"
#include "StartScene.h"

#include "Core/Collision.h"
#include "Core/ComponentTypes.h"
#include "Core/Constant.h"
#include "Core/Entity/Entity.h"
#include "Core/Input.h"

#include "MenuComponentTypes.h"

void StartScene::Initialize()
{
	mBgSkyTexture.Initialize(GetHelper(), "Resource/Title/Scene/TownSky_Xmas.png");
	mLogoTexture.Initialize(GetHelper(), "Resource/Title/Logo/0.png");
	
	mStartButtonNormalTexture.Initialize(GetHelper(), "Resource/Title/Button/Start/0.png");
	mStartButtonHoverTexture.Initialize(GetHelper(), "Resource/Title/Button/Start/1.png");

	mExitButtonNormalTexture.Initialize(GetHelper(), "Resource/Title/Button/Exit/0.png");
	mExitButtonHoverTexture.Initialize(GetHelper(), "Resource/Title/Button/Exit/1.png");

	// Camera
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(Camera());
		entity->AddComponent(Transform());
	}

	// BackGround
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(NextBackGroundTag());
		entity->AddComponent(Color());

		Image image{};
		image.texture = &mBgSkyTexture;
		image.layer = uint32_t(Layer::BackGround);
		entity->AddComponent(image);

		Transform transform{};
		transform.scale = { .width = 3.5f, .height = 3.5f };
		entity->AddComponent(transform);

		Active active{};
		active.isValue = true;
		entity->AddComponent(active);
	}

	// Logo
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(LogoTag());
		entity->AddComponent(Color());

		Image image{};
		image.texture = &mLogoTexture;
		image.layer = uint32_t(Layer::Logo);
		entity->AddComponent(image);

		Transform transform{};
		transform.position = { .x = 0.0f, .y = Constant::Get().GetHalfHeight() - 180.0f };
		transform.scale = { .width = 2.0f, .height = 2.0f };
		entity->AddComponent(transform);

		Active active{};
		active.isValue = true;
		entity->AddComponent(active);
	}

	// Start Button
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(StartTag());
		entity->AddComponent(Button());
		entity->AddComponent(Direction());
		entity->AddComponent(Color());
		entity->AddComponent(DebugActive());
		entity->AddComponent(DebugColor());

		CollisionDetector collider(static_cast<uint32_t>(StartScene::CollisionLayer::StartButton));
		collider.CollisionLayerMask.set(uint32_t(StartScene::CollisionLayer::MouseCursor));
		entity->AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.offset = { .x = 0.0f, .y = 0.0f };
		boxCollider.size = { .width = 37.0f, .height = 15.0f };
		entity->AddComponent(boxCollider);

		Image image{};
		image.texture = &mStartButtonNormalTexture;
		image.layer = uint32_t(Layer::Button);
		entity->AddComponent(image);

		Transform transform{};
		transform.position = { .x = 0.0f, .y = -Constant::Get().GetHalfHeight() + 200.0f };
		transform.scale = { .width = 3.0f, .height = 3.0f };
		entity->AddComponent(transform);

		Active active{};
		active.isValue = true;
		entity->AddComponent(active);
	}

	// Exit Button
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(ExitTag());
		entity->AddComponent(Button());
		entity->AddComponent(Direction());
		entity->AddComponent(Color());
		entity->AddComponent(DebugActive());
		entity->AddComponent(DebugColor());

		CollisionDetector collider(static_cast<uint32_t>(StartScene::CollisionLayer::ExitButton));
		collider.CollisionLayerMask.set(uint32_t(StartScene::CollisionLayer::MouseCursor));
		entity->AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.offset = { .x = 0.0f, .y = 0.0f };
		boxCollider.size = { .width = 37.0f, .height = 15.0f };
		entity->AddComponent(boxCollider);

		Image image{};
		image.texture = &mExitButtonNormalTexture;
		image.layer = uint32_t(Layer::Button);
		entity->AddComponent(image);

		Transform transform{};
		transform.position = { .x = 0.0f, .y = -Constant::Get().GetHalfHeight() + 120.0f };
		transform.scale = { .width = 3.0f, .height = 3.0f };
		entity->AddComponent(transform);

		Active active{};
		active.isValue = true;
		entity->AddComponent(active);
	}

	// Mouse
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(MouseCursorTag());
		entity->AddComponent(Transform());
		entity->AddComponent(Direction());
		entity->AddComponent(DebugActive());
		entity->AddComponent(DebugColor());

		CollisionDetector collider(static_cast<uint32_t>(StartScene::CollisionLayer::MouseCursor));
		collider.CollisionLayerMask.set(uint32_t(StartScene::CollisionLayer::MouseCursor));
		entity->AddComponent(collider);

		BoxCollider boxCollider{};
		boxCollider.offset = { .x = 3.0f, .y = -7.0f };
		boxCollider.size = { .width = 18.0f, .height = 20.0f };
		entity->AddComponent(boxCollider);

		Active active{};
		active.isValue = true;
		entity->AddComponent(active);
	}
}

bool StartScene::Update(const float deltaTime)
{	
	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
	{
		quitEvent();
	}

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
	}

	// 버튼을 업데이트한다.
	{
		// 시작 버튼
		const Entity* startEntity = getEntity<StartTag>();
		if (const Button* button = startEntity->GetComponent<Button>();
			not button->isNormal)
		{
			if (Input::Get().GetMouseButtonDown(SDL_BUTTON_LEFT))
			{
				mIsFade = true;
			}
		}

		if (mIsFade)
		{
			const Entity* backGroundEntity = getEntity<NextBackGroundTag>();
			const Entity* LogoEntity = getEntity<LogoTag>();
			const Entity* exitEntity = getEntity<ExitTag>();

			const Uint8 fadeOutTime = Uint8(200.0f * deltaTime);

			Color* backGroundColor = backGroundEntity->GetComponent<Color>();
			backGroundColor->a -= fadeOutTime;

			Color* logoColor = LogoEntity->GetComponent<Color>();
			logoColor->a -= fadeOutTime;

			Color* startColor = startEntity->GetComponent<Color>();
			startColor->a -= fadeOutTime;

			Color* exitColor = exitEntity->GetComponent<Color>();
			exitColor->a -= fadeOutTime;

			if (backGroundColor->a <= 0)
			{
				mIsUpdate = false;
			}
		}

		// 종료 버튼
		const Entity* exitEntity = getEntity<ExitTag>();
		if (const Button* button = exitEntity->GetComponent<Button>();
			not button->isNormal)
		{
			if (Input::Get().GetMouseButtonDown(SDL_BUTTON_LEFT))
			{
				quitEvent();
			}
		}
	}

	// 마우스 좌표를 업데이트한다.
	{
		Entity* mouseEntity = getEntity<MouseCursorTag>();
		Transform* transform = mouseEntity->GetComponent<Transform>();
		transform->position = getScreenMousePosition();
	}

	// 충돌을 업데이트한다.
	{
		// 시작 버튼
		{
			Entity* startEntity = getEntity<StartTag>();
			Entity* mouseEntity = getEntity<MouseCursorTag>();

			if (Collision::Get().IsCollisionEnter(*startEntity, *mouseEntity))
			{
				Image* image = startEntity->GetComponent<Image>();
				image->texture = &mStartButtonHoverTexture;

				Button* button = startEntity->GetComponent<Button>();
				button->isNormal = false;
			}
			else if (Collision::Get().IsCollisionExit(*startEntity, *mouseEntity))
			{
				Image* image = startEntity->GetComponent<Image>();
				image->texture = &mStartButtonNormalTexture;

				Button* button = startEntity->GetComponent<Button>();
				button->isNormal = true;
			}
		}

		// 종료 버튼
		{
			Entity* exitEntity = getEntity<ExitTag>();
			Entity* mouseEntity = getEntity<MouseCursorTag>();

			if (Collision::Get().IsCollisionEnter(*exitEntity, *mouseEntity))
			{
				Image* image = exitEntity->GetComponent<Image>();
				image->texture = &mExitButtonHoverTexture;

				Button* button = exitEntity->GetComponent<Button>();
				button->isNormal = false;
			}
			else if (Collision::Get().IsCollisionExit(*exitEntity, *mouseEntity))
			{
				Image* image = exitEntity->GetComponent<Image>();
				image->texture = &mExitButtonNormalTexture;

				Button* button = exitEntity->GetComponent<Button>();
				button->isNormal = true;
			}
		}
	}

	Collision::Get().UpdateEntityPairs();

	return mIsUpdate;
}

void StartScene::Finalize()
{
	mBgSkyTexture.Finalize();
	mLogoTexture.Finalize();
	
	mStartButtonNormalTexture.Finalize();
	mStartButtonHoverTexture.Finalize();

	mExitButtonNormalTexture.Finalize();
	mExitButtonHoverTexture.Finalize();
}

template <typename T>
Entity* StartScene::getEntity() const
{
	Entity* foundEntity = nullptr;

	for (Entity* entity : GetEntityWorld()->GetAllEntities())
	{
		if (not entity->HasComponent<T>())
		{
			continue;
		}

		if (foundEntity != nullptr)
		{
			assert(false and "여러 Entity가 존재합니다.");
			break;
		}

		foundEntity = entity;
	}

	return foundEntity;
}

template<typename T>
std::vector<Entity*> StartScene::getEntities() const
{
	std::vector<Entity*> result{};

	for (Entity* entity : GetEntityWorld()->GetAllEntities())
	{
		if (not entity->HasComponent<T>())
		{
			continue;
		}

		result.push_back(entity);
	}

	return result;
}

Point StartScene::getScreenMousePosition() const
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

void StartScene::quitEvent()
{
	SDL_Event quit_event{};
	quit_event.type = SDL_QUIT;
	SDL_PushEvent(&quit_event);
}