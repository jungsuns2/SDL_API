#include "pch.h"
#include "Core.h"

#include "Entity/Entity.h"
#include "Entity/EntityWorld.h"

#include "Clip.h"
#include "Collision.h"
#include "Constant.h"
#include "Texture.h"

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
		Transform* cameraTransform = cameraSystem(entityWorld);

		imageRenderingSystem(entityWorld, cameraTransform);
		animatorRenderingSystem(entityWorld, cameraTransform, deltaTime);

		colliderAnimatorRenderingSystem(entityWorld, cameraTransform);
		colliderImageRenderingSystem(entityWorld, cameraTransform);

		labelRenderingSystem(entityWorld);

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

Transform* Core::cameraSystem(const EntityWorld* entityWorld)
{
	assert(entityWorld != nullptr);

	Transform* cameraTransform = nullptr;

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

	assert(cameraTransform != nullptr and "등록된 카메라가 없습니다.");

	return cameraTransform;
}

void Core::textureSystem(const TextureSystemDesc& desc)
{
	const Scale textureScale = desc.textureScale;
	Transform* textureTransform = desc.textureTransform;
	Transform* cameraTransform = desc.cameraTransform;
	SDL_FRect* rect = desc.rect;
	SDL_FPoint* angleCenter = desc.angleCenter;

	Point center =
	{
		.x = textureTransform->center.x + 0.5f,
		.y = 1.0f - (textureTransform->center.y + 0.5f),
	};

	if (textureTransform->flip == SDL_FLIP_HORIZONTAL)
	{
		center.x = 1.0f - center.x;
	}

	const Point offset =
	{
		.x = center.x * (textureScale.width * textureTransform->scale.width),
		.y = center.y * (textureScale.height * textureTransform->scale.height)
	};

	const Point cameraCenter =
	{
		.x = (Constant::Get().GetWidth() - 1.0f) * 0.5f,
		.y = (Constant::Get().GetHeight() - 1.0f) * 0.5f,
	};

	const Point cameraOffset =
	{
		.x = cameraCenter.x - cameraTransform->position.x,
		.y = cameraCenter.y + cameraTransform->position.y,
	};

	rect->x = cameraOffset.x + textureTransform->position.x - offset.x;
	rect->y = cameraOffset.y - textureTransform->position.y - offset.y;
	rect->w = textureScale.width * textureTransform->scale.width;
	rect->h = textureScale.height * textureTransform->scale.height;

	angleCenter->x = offset.x;
	angleCenter->y = offset.y;
}

void Core::drawSystem(const DrawSystemDesc& desc)
{
	const Scale textureScale = desc.textureScale;
	Transform* transform = desc.transform;
	Transform* cameraTransform = desc.cameraTransform;
	SDL_FRect* rect = desc.rect;

	const Point center =
	{
		.x = (transform->center.x + 1.0f) * 0.5f,
		.y = (transform->center.y + 1.0f) * 0.5f,
	};

	const Point offset =
	{
		.x = center.x * (textureScale.width * transform->scale.width),
		.y = center.y * (textureScale.height * transform->scale.height)
	};

	const Point cameraCenter =
	{
		.x = (Constant::Get().GetWidth() - 1.0f) * 0.5f,
		.y = (Constant::Get().GetHeight() - 1.0f) * 0.5f,
	};

	const Point cameraOffset =
	{
		.x = cameraCenter.x - cameraTransform->position.x,
		.y = cameraCenter.y + cameraTransform->position.y,
	};

	rect->x = cameraOffset.x + transform->position.x - offset.x;
	rect->y = cameraOffset.y - transform->position.y - offset.y;
	rect->w = textureScale.width * transform->scale.width;
	rect->h = textureScale.height * transform->scale.height;
}

void Core::imageRenderingSystem(const EntityWorld* entityWorld, Transform* cameraTransform)
{
	assert(entityWorld != nullptr);
	assert(cameraTransform != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntites())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<Image>()
			or not entity->HasComponent<Active>()
			or not entity->HasComponent<Color>())
		{
			continue;
		}

		const Active* active = entity->GetComponent<Active>();
		if (not active->isValue)
		{
			continue;
		}

		Transform* transform = entity->GetComponent<Transform>();
		Color* color = entity->GetComponent<Color>();
		SDL_FRect rect{};
		SDL_FPoint angleCenter{};

		const Image* image = entity->GetComponent<Image>();
		textureSystem
		(
			TextureSystemDesc
			{
				.textureScale = {.width = float(image->texture->GetWidth()), .height = float(image->texture->GetHeight()) },
				.textureTransform = transform,
				.cameraTransform = cameraTransform,
				.rect = &rect,
				.angleCenter = &angleCenter
			}
		);

		SDL_SetTextureColorMod(image->texture->GetTexture(), color->r, color->g, color->b);
		SDL_SetTextureAlphaMod(image->texture->GetTexture(), color->a);
		SDL_RenderCopyExF(mRenderer, image->texture->GetTexture(), nullptr, &rect, transform->angle, &angleCenter, transform->flip);
	}
}

void Core::animatorRenderingSystem(const EntityWorld* entityWorld, Transform* cameraTransform, const float deltaTime)
{
	assert(entityWorld != nullptr);
	assert(cameraTransform != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntites())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<Animator>()
			or not entity->HasComponent<Active>()
			or not entity->HasComponent<Color>())
		{
			continue;
		}

		Active* active = entity->GetComponent<Active>();
		if (not active->isValue)
		{
			continue;
		}

		Animator* animator = entity->GetComponent<Animator>();
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

		Transform* transform = entity->GetComponent<Transform>();
		transform->center = frame.center;

		Color* color = entity->GetComponent<Color>();
		SDL_FRect rect{};
		SDL_FPoint angleCenter{};

		textureSystem
		(
			TextureSystemDesc
			{
				.textureScale = {.width = float(frame.texture->GetWidth()), .height = float(frame.texture->GetHeight()) },
				.textureTransform = transform,
				.cameraTransform = cameraTransform,
				.rect = &rect,
				.angleCenter = &angleCenter
			}
		);

		SDL_SetTextureColorMod(frame.texture->GetTexture(), color->r, color->g, color->b);
		SDL_SetTextureAlphaMod(frame.texture->GetTexture(), color->a);
		SDL_RenderCopyExF(mRenderer, frame.texture->GetTexture(), nullptr, &rect, transform->angle, &angleCenter, transform->flip);
	}
}

void Core::colliderAnimatorRenderingSystem(const EntityWorld* entityWorld, Transform* cameraTransform)
{
	//assert(entityWorld != nullptr);
	//assert(cameraTransform != nullptr);

	//for (const Entity* entity : entityWorld->GetAllEntites())
	//{
	//	if (not entity->HasComponent<Transform>()
	//		or not entity->HasComponent<Animator>()
	//		or not entity->HasComponent<Collider>()
	//		or not entity->HasComponent<DebugActive>()
	//		or not entity->HasComponent<DebugColor>()
	//		or not entity->HasComponent<Color>())
	//	{
	//		continue;
	//	}

	//	DebugActive* active = entity->GetComponent<DebugActive>();
	//	if (not active->value)
	//	{
	//		continue;
	//	}

	//	Animator* animator = entity->GetComponent<Animator>();
	//	const Clip* clip = animator->clipState;
	//	const std::vector<Clip::Frame>& frames = clip->GetAllFrames();
	//	const Clip::Frame& frame = frames[0];

	//	Transform* transform = entity->GetComponent<Transform>();
	//	SDL_FRect rect{};

	//	drawSystem
	//	(
	//		{
	//			.textureScale = {.width = float(frame.texture->GetWidth()), .height = float(frame.texture->GetHeight()) },
	//			.transform = transform,
	//			.cameraTransform = cameraTransform,
	//			.rect = &rect,
	//		}
	//	);

	//	SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
	//	DebugColor* color = entity->GetComponent<DebugColor>();
	//	SDL_SetRenderDrawColor(mRenderer, color->r, color->g, color->b, color->a);
	//	SDL_RenderFillRectF(mRenderer, &rect);
	//}
}

void Core::colliderImageRenderingSystem(const EntityWorld* entityWorld, Transform* cameraTransform)
{
	assert(entityWorld != nullptr);
	assert(cameraTransform != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntites())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<Image>()
			/*or not entity->HasComponent<Collider>()*/
			or not entity->HasComponent<DebugActive>()
			or not entity->HasComponent<DebugColor>()
			or not entity->HasComponent<Color>())
		{
			continue;
		}

		DebugActive* active = entity->GetComponent<DebugActive>();
		if (not active->value)
		{
			continue;
		}

		Image* image = entity->GetComponent<Image>();

		Transform* transform = entity->GetComponent<Transform>();
		SDL_FRect rect{};

		drawSystem
		(
			DrawSystemDesc
			{
				.textureScale = {.width = float(image->texture->GetWidth()), .height = float(image->texture->GetHeight()) },
				.transform = transform,
				.cameraTransform = cameraTransform,
				.rect = &rect,
			}
			);

		SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
		DebugColor* color = entity->GetComponent<DebugColor>();
		SDL_SetRenderDrawColor(mRenderer, color->r, color->g, color->b, color->a);
		SDL_RenderFillRectF(mRenderer, &rect);
	}
}

void Core::labelRenderingSystem(const EntityWorld* entityWorld)
{
	assert(entityWorld != nullptr);

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

		Color* color = entity->GetComponent<Color>();
		SDL_SetTextureColorMod(label->texture, color->r, color->g, color->b);
		SDL_SetTextureAlphaMod(label->texture, color->a);
		SDL_RenderCopyF(mRenderer, label->texture, nullptr, &rect);
	}
}

SDL_Window* Core::GetWindow() const
{
	return mWindow;
}