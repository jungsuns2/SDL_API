#include "pch.h"

#include "Entity/Entity.h"
#include "Entity/EntityWorld.h"

#include "Clip.h"
#include "ComponentTypes.h"
#include "Constant.h"
#include "Core.h"

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
		SDL_RenderClear(mRenderer);	// 화면을 지정색으로 채운다.

		if (not mScene->Update(deltaTime))
		{
			return false;
		}

		const Camera* camera = mScene->GetCamera();

		const EntityWorld* entityWorld = mScene->GetEntityWorld();
		for (const Entity* entity : entityWorld->GetAllEntites())
		{
			if (not entity->HasComponent<Transform>()
				or not entity->HasComponent<Material>())
			{
				continue;
			}

			const Transform* transform = entity->GetComponent<Transform>();
			const Material* material = entity->GetComponent<Material>();

			const Point centerOffset =
			{
				.x = (Constant::Get().GetWidth() - 1.0f) * 0.5f,
				.y = (Constant::Get().GetHeight() - 1.0f) * 0.5f,
			};

			const SDL_FRect rect =
			{
				.x = transform->position.x - camera->position.x + centerOffset.x,
				.y = transform->position.y - camera->position.y + centerOffset.y,
				.w = material->texture->GetWidth() * transform->scale.width,
				.h = material->texture->GetHeight() * transform->scale.height,
			};

			SDL_RenderCopyExF(mRenderer, material->texture->GetTexture(), nullptr, &rect, 0.0f, nullptr, SDL_FLIP_NONE);
		}

		for (const Entity* entity : entityWorld->GetAllEntites())
		{
			if (not entity->HasComponent<Transform>()
				or not entity->HasComponent<Animator>())
			{
				continue;
			}

			const Transform* transform = entity->GetComponent<Transform>();
			Animator* animator = entity->GetComponent<Animator>();

			Clip* clip = animator->clipState;
			std::vector<Clip::Frame>& frames = clip->GetAllFrames();

			uint32_t index = animator->frameIndex;
			
			animator->elapsedTime += deltaTime;

			if (animator->elapsedTime >= frames[index].durationTime)
			{
				animator->elapsedTime = 0.0f;
				++animator->frameIndex;

				if (animator->frameIndex >= frames.size())
				{
					if (clip->GetLoop())
					{
						animator->frameIndex = 0;
					}
					else
					{
						animator->frameIndex = frames.size() - 1;
					}
				}
			}

			const Point centerOffset =
			{
				.x = (Constant::Get().GetWidth() - 1.0f) * 0.5f,
				.y = (Constant::Get().GetHeight() - 1.0f) * 0.5f,
			};

			const SDL_FRect rect =
			{
				.x = transform->position.x - camera->position.x + centerOffset.x,
				.y = transform->position.y - camera->position.y + centerOffset.y,
				.w = frames[index].texture->GetWidth() * transform->scale.width,
				.h = frames[index].texture->GetHeight() * transform->scale.height,
			};

			SDL_RenderCopyExF(mRenderer, frames[index].texture->GetTexture(), nullptr, &rect, 0.0f, nullptr, SDL_FLIP_NONE);
		}

		for (const Entity* entity : entityWorld->GetAllEntites())
		{
			if (not entity->HasComponent<Transform>()
				or not entity->HasComponent<Label>())
			{
				continue;
			}

			Transform* transform = entity->GetComponent<Transform>();
			Label* label = entity->GetComponent<Label>();

			SDL_FRect rect =
			{
				.x = transform->position.x,
				.y = transform->position.y,
				.w = label->width,
				.h = label->height,
			};

			SDL_RenderCopyF(mRenderer, label->texture, nullptr, &rect);
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