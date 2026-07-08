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

	// Camera
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(Camera());
		entity->AddComponent(Transform());
	}

	// BackGround
	{
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(BackGroundTag());
		entity->AddComponent(Image());
		entity->AddComponent(Transform());
		entity->AddComponent(Active());

		Image* image = entity->GetComponent<Image>();
		image->texture = &mBgSkyTexture;

		Transform* transform = entity->GetComponent<Transform>();
		transform->scale = { .width = 3.5f, .height = 3.5f };

		Active* active = entity->GetComponent<Active>();
		active->isValue = true;
	}

	// Logo
	{
		// TODO: 로고 사이즈가 너무 큼, 그리고 png 파일이 아님;
		Entity* entity = GetEntityWorld()->AddEntity(new Entity());
		entity->AddComponent(LogoTag());
		entity->AddComponent(Image());
		entity->AddComponent(Transform());
		entity->AddComponent(Active());

		Image* image = entity->GetComponent<Image>();
		image->texture = &mLogoTexture;

		Transform* transform = entity->GetComponent<Transform>();
		transform->position = { .x = 0.0f, .y = Constant::Get().GetHalfHeight() - 180.0f };
		transform->scale = { .width = 2.0f, .height = 2.0f };

		Active* active = entity->GetComponent<Active>();
		active->isValue = true;
	}
}

bool StartScene::Update(const float deltaTime)
{	if (Input::Get().GetKeyDown(SDL_SCANCODE_ESCAPE))
	{
		SDL_Event quit_event{};
		quit_event.type = SDL_QUIT;
		SDL_PushEvent(&quit_event);
	}

	if (Input::Get().GetKeyDown(SDL_SCANCODE_T))
	{
		mIsUpdate = false;
	}

	Collision::Get().UpdateEntityPairs();

	return mIsUpdate;
}

void StartScene::Finalize()
{
	mBgSkyTexture.Finalize();
	mLogoTexture.Finalize();
}