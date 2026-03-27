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

		const EntityWorld* entityWorld = mScene->GetEntityWorld();

		Transform* cameraTransform = nullptr;
		{
			for (const Entity* entity : entityWorld->GetAllEntites())
			{
				if (not entity->HasComponent<Camera>()
					or not entity->GetComponent<Transform>())
				{
					continue;
				}

				cameraTransform = entity->GetComponent<Transform>();
				break;
			}

			assert(cameraTransform != nullptr && "등록된 카메라가 없습니다.");
		}

		for (const Entity* entity : entityWorld->GetAllEntites())
		{
			if (not entity->HasComponent<Transform>()
				or not entity->HasComponent<Material>())
			{
				continue;
			}

			const Material* material = entity->GetComponent<Material>();
			if (not material->active)
			{
				continue;
			}

			const Point centerOffset =
			{
				.x = (Constant::Get().GetWidth() - 1.0f) * 0.5f,
				.y = (Constant::Get().GetHeight() - 1.0f) * 0.5f,
			};

			const Transform* transform = entity->GetComponent<Transform>();
			const SDL_FRect rect =
			{
				.x = centerOffset.x + transform->position.x - cameraTransform->position.x,
				.y = centerOffset.y + transform->position.y - cameraTransform->position.y,
				.w = material->texture->GetWidth() * transform->scale.width,
				.h = material->texture->GetHeight() * transform->scale.height,
			};

			SDL_RenderCopyExF(mRenderer, material->texture->GetTexture(), nullptr, &rect, transform->angle, nullptr, SDL_FLIP_NONE);
		}

		for (const Entity* entity : entityWorld->GetAllEntites())
		{
			if (not entity->HasComponent<Transform>()
				or not entity->HasComponent<Animator>())
			{
				continue;
			}

			Animator* animator = entity->GetComponent<Animator>();
			if (not animator->active)
			{
				continue;
			}

			const Clip* clip = animator->clipState;
			const std::vector<Clip::Frame>& frames = clip->GetAllFrames();
			const Clip::Frame& frame = frames[animator->frameIndex];
			
			animator->elapsedTime += deltaTime;
			if (animator->elapsedTime >= frame.durationTime)
			{
				if (++animator->frameIndex >= frames.size())
				{
					if (clip->IsLoop())
					{
						animator->frameIndex = 0;
					}
					else
					{
						animator->frameIndex = uint32_t(frames.size() - 1);
					}
				}

				animator->elapsedTime = 0.0f;
			}

			const Point centerOffset =
			{
				.x = (Constant::Get().GetWidth() - 1.0f) * 0.5f,
				.y = (Constant::Get().GetHeight() - 1.0f) * 0.5f,
			};

			const Transform* transform = entity->GetComponent<Transform>();
			const SDL_FRect rect =
			{
				.x = centerOffset.x + transform->position.x - cameraTransform->position.x,
				.y = centerOffset.y + transform->position.y - cameraTransform->position.y,
				.w = frame.texture->GetWidth() * transform->scale.width,
				.h = frame.texture->GetHeight() * transform->scale.height,
			};

			SDL_RenderCopyExF(mRenderer, frame.texture->GetTexture(), nullptr, &rect, transform->angle, nullptr, transform->flip);
		}

		for (const Entity* entity : entityWorld->GetAllEntites())
		{
			if (not entity->HasComponent<Transform>()
				or not entity->HasComponent<Label>())
			{
				continue;
			}

			const Label* label = entity->GetComponent<Label>();
			if (not label->active)
			{
				continue;
			}

			const Transform* transform = entity->GetComponent<Transform>();
			const SDL_FRect rect =
			{
				.x = transform->position.x,
				.y = transform->position.y,
				.w = label->width,
				.h = label->height,
			};

			SDL_RenderCopyF(mRenderer, label->texture, nullptr, &rect);
		}

		SDL_RenderPresent(mRenderer); // 화면에 출력한다.
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