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
	SDL_SetRenderDrawColor(mRenderer, 255, 174, 201, 255);
	SDL_RenderClear(mRenderer);	// 화면을 지정색으로 채운다.

	if (not mScene->Update(deltaTime))
	{
		return false;
	}

	const EntityWorld* entityWorld = mScene->GetEntityWorld();
	Transform* cameraTransform = cameraSystem(entityWorld);

	updateAnimator(entityWorld, deltaTime);
	drawImages(entityWorld, cameraTransform);

	colliderAnimatorRenderingSystem(entityWorld, cameraTransform);
	colliderImageRenderingSystem(entityWorld, cameraTransform);

	labelUIRenderingSystem(entityWorld, cameraTransform);
	labelRenderingSystem(entityWorld, cameraTransform);

	SDL_RenderPresent(mRenderer); // 화면에 출력한다.

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

void Core::updateAnimator(const EntityWorld* entityWorld, const float deltaTime)
{
	assert(entityWorld != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntites())
	{
		if (not entity->HasComponent<Animator>())
		{
			continue;
		}

		if (entity->HasComponent<Active>())
		{
			if (const Active* active = entity->GetComponent<Active>();
				not active->isValue)
			{
				continue;
			}
		}

		Animator* animator = entity->GetComponent<Animator>();

		const Clip* clip = animator->clipState;
		assert(clip != nullptr and "animator 컴포넌트에 설정된 clip이 없습니다.");

		const std::vector<Clip::Frame>& frames = clip->GetAllFrames();
		assert(frames.size() > 0 and "clip 컴포넌트에 설정된 frame이 없습니다.");

		const Clip::Frame* frame = &frames[animator->frameIndex];

		animator->elapsedTime += deltaTime;
		if (animator->elapsedTime >= frame->durationTime)
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

				frame = &frames[animator->frameIndex];
			}

			animator->elapsedTime = 0.0f;
		}

		if (entity->HasComponent<Transform>())
		{
			Transform* transform = entity->GetComponent<Transform>();
			transform->center = frame->center;
		}

		if (entity->HasComponent<Image>())
		{
			Image* image = entity->GetComponent<Image>();
			image->texture = frame->texture;
		}
	}
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
	const Transform* textureTransform = desc.textureTransform;
	const Transform* cameraTransform = desc.cameraTransform;
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

void Core::drawImages(const EntityWorld* entityWorld, const Transform* cameraTransform)
{
	assert(entityWorld != nullptr);
	assert(cameraTransform != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntites())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<Image>())
		{
			continue;
		}

		if (entity->HasComponent<Active>())
		{
			if (const Active* active = entity->GetComponent<Active>();
				not active->isValue)
			{
				continue;
			}
		}

		const Transform* transform = entity->GetComponent<Transform>();
		const Image* image = entity->GetComponent<Image>();
		const Texture* texture = image->texture;
		assert(texture != nullptr and "image 컴포넌트에 설정된 texture가 유효하지 않습니다.");

		SDL_FRect rect{};
		SDL_FPoint angleCenter{};

		textureSystem(TextureSystemDesc
			{
				.textureScale = {.width = float(texture->GetWidth()), .height = float(texture->GetHeight()) },
				.textureTransform = transform,
				.cameraTransform = cameraTransform,
				.rect = &rect,
				.angleCenter = &angleCenter
			});

		Color color{};
		if (entity->HasComponent<Color>())
		{
			color = *entity->GetComponent<Color>();
		}

		SDL_SetTextureColorMod(texture->GetTexture(), color.r, color.g, color.b);
		SDL_SetTextureAlphaMod(texture->GetTexture(), color.a);
		SDL_RenderCopyExF(mRenderer, texture->GetTexture(), nullptr, &rect, transform->angle, &angleCenter, transform->flip);
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

void Core::labelRenderingSystem(const EntityWorld* entityWorld, Transform* cameraTransform)
{
	assert(entityWorld != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntites())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<Label>()
			or entity->HasComponent<UI>())
		{
			continue;
		}

		if (entity->HasComponent<Active>())
		{
			if (const Active* active = entity->GetComponent<Active>();
				not active->isValue)
			{
				continue;
			}
		}

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

		Transform* transform = entity->GetComponent<Transform>();
		Label* label = entity->GetComponent<Label>();

		Color color{};
		if (entity->HasComponent<Color>())
		{
			color = *entity->GetComponent<Color>();
		}

		SDL_Surface* surface = TTF_RenderText_Blended(label->font->GetFont(), label->text.c_str(), { color.r, color.g, color.b , color.a });

		const Scale scale =
		{
			.width = float(surface->w),
			.height = float(surface->h)
		};


		label->texture = SDL_CreateTextureFromSurface(mRenderer, surface);
		SDL_FreeSurface(surface);

		if (label->texture != nullptr)
		{
			const SDL_Rect rect =
			{
				.x = int(cameraOffset.x + transform->position.x),
				.y = int(cameraOffset.y - transform->position.y),
				.w = int(scale.width),
				.h = int(scale.height),
			};

			SDL_RenderCopy(mRenderer, label->texture, nullptr, &rect);

			SDL_DestroyTexture(label->texture);
			label->texture = nullptr;
		}
	}
}

void Core::labelUIRenderingSystem(const EntityWorld* entityWorld, Transform* cameraTransform)
{
	assert(entityWorld != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntites())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<Label>()
			or not entity->HasComponent<UI>())
		{
			continue;
		}

		if (entity->HasComponent<Active>())
		{
			if (const Active* active = entity->GetComponent<Active>();
				not active->isValue)
			{
				continue;
			}
		}

		const Transform* transform = entity->GetComponent<Transform>();
		Label* label = entity->GetComponent<Label>();

		Color color{};
		if (entity->HasComponent<Color>())
		{
			color = *entity->GetComponent<Color>();
		}

		SDL_Surface* surface = TTF_RenderText_Blended(label->font->GetFont(), label->text.c_str(), { color.r, color.g, color.b , color.a });
		
		const Scale scale =
		{
			.width = float(surface->w),
			.height = float(surface->h)
		};


		label->texture = SDL_CreateTextureFromSurface(mRenderer, surface);
		SDL_FreeSurface(surface);
		
		if (label->texture != nullptr)
		{
			const SDL_Rect rect =
			{
				.x = int(transform->position.x),
				.y = int(transform->position.y),
				.w = int(scale.width),
				.h = int(scale.height),
			};

			SDL_RenderCopy(mRenderer, label->texture, nullptr, &rect);

			SDL_DestroyTexture(label->texture);
			label->texture = nullptr;
		}
	}
}

SDL_Window* Core::GetWindow() const
{
	return mWindow;
}