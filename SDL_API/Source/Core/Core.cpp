#include "pch.h"
#include "Constant.h"
#include "Core.h"
#include "Entity/Entity.h"
#include "Entity/EntityWorld.h"
#include "ComponentTypes.h"

void Core::Initialize(Scene* scene)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	mWindow = SDL_CreateWindow("Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		Constant::Get().GetWidth(), Constant::Get().GetHeight(), SDL_WINDOW_SHOWN);

	// SDL_RENDERER_PRESENTVSYNC: 모니터 주사율에 맞춘다.
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	mHelper.Initialize(mRenderer);

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	TTF_Init();

	assert(scene != nullptr);
	
	if (mScene != nullptr)
	{
		mScene->Finalize();
		delete mScene;
		mScene = nullptr;
	}

	mScene = scene;
	mScene->_SetHelper(&mHelper);
	mScene->Initialize();
}

bool Core::Update(const float deltaTime)
{
	// Render
	{
		SDL_SetRenderDrawColor(mRenderer, 255, 174, 201, 255);
		SDL_RenderClear(mRenderer);		// 화면을 지정색으로 채운다.

		if (not mScene->Update(deltaTime))
		{
			return false;
		}

		const EntityWorld* entityWorld = mScene->_GetEntityWorld();

		for (Entity* entity : entityWorld->GetAllEntites())
		{

			if (entity->HasComponent<Material>())
			{
				continue;
			}

			Material* material = entity->GetComponent<Material>();
			Transform* transform = entity->GetComponent<Transform>();			

			SDL_FRect rect = 
			{
				.x = transform->position.x,
				.y = transform->position.y,
				.w = material->texture->GetWidth()  * transform->scale.width,
				.h = material->texture->GetHeight() * transform->scale.height,
			};

			printf("%f, %f \n", transform->position.x, transform->position.y);

			SDL_RenderCopyF(mRenderer, material->texture->GetTexture(), nullptr, &rect);
		}

		SDL_RenderPresent(mRenderer);	// 화면에 출력한다.
	}

	return true;
}

void Core::Finalize()
{
	mScene->Finalize();

	IMG_Quit();

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);

	SDL_Quit();
}

SDL_Window* Core::GetWindow() const
{
	return mWindow;
}