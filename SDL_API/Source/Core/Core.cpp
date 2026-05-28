#include "pch.h"
#include "Core.h"

#include "Entity/Entity.h"
#include "Entity/EntityWorld.h"

#include "Clip.h"
#include "Collision.h"
#include "Constant.h"

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

	mColliderTexture.Initialize(&mHelper, "Resource/WhileRect.png");
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
	mCameraTransform = cameraSystem(entityWorld);

	updateAnimator(entityWorld, deltaTime);
	drawImages(entityWorld);

	colliderImageRenderingSystem(entityWorld);

	labelUIRenderingSystem(entityWorld);
	labelRenderingSystem(entityWorld);

	SDL_RenderPresent(mRenderer); // 화면에 출력한다.

	return true;
}

void Core::Finalize()
{
	mScene->Finalize();

	delete mCameraTransform;
	mCameraTransform = nullptr;

	mColliderTexture.Finalize();

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

Point Core::getCameraOffset() const
{
	const Point cameraCenter =
	{
		.x = (Constant::Get().GetWidth() - 1.0f) * 0.5f,
		.y = (Constant::Get().GetHeight() - 1.0f) * 0.5f,
	};

	const Point result =
	{
		.x = cameraCenter.x - mCameraTransform->position.x,
		.y = cameraCenter.y + mCameraTransform->position.y,
	};

	return result;
}

void Core::textureSystem(const TextureSystemDesc& desc)
{
	const Scale textureScale = desc.textureScale;
	const Transform* textureTransform = desc.textureTransform;
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

	rect->x = getCameraOffset().x + textureTransform->position.x - offset.x;
	rect->y = getCameraOffset().y - textureTransform->position.y - offset.y;
	rect->w = textureScale.width * textureTransform->scale.width;
	rect->h = textureScale.height * textureTransform->scale.height;

	angleCenter->x = offset.x;
	angleCenter->y = offset.y;
}

void Core::drawSystem(const DrawSystemDesc& desc)
{
	const Scale textureScale = desc.textureScale;
	Transform* transform = desc.transform;
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

	rect->x = getCameraOffset().x + transform->position.x - offset.x;
	rect->y = getCameraOffset().y - transform->position.y - offset.y;
	rect->w = textureScale.width * transform->scale.width;
	rect->h = textureScale.height * transform->scale.height;
}

void Core::drawImages(const EntityWorld* entityWorld)
{
	assert(entityWorld != nullptr);

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

		textureSystem
		(
			TextureSystemDesc
			{
				.textureScale = {.width = float(texture->GetWidth()), .height = float(texture->GetHeight()) },
				.textureTransform = transform,
				.rect = &rect,
				.angleCenter = &angleCenter
			}
		);

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

void Core::colliderImageRenderingSystem(const EntityWorld* entityWorld)
{
	assert(entityWorld != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntites())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<Image>()
			or not entity->HasComponent<DebugActive>()
			or not entity->HasComponent<DebugColor>())
		{
			continue;
		}

		if (const DebugActive* active = entity->GetComponent<DebugActive>();
			not active->isValue)
		{
			continue;
		}

		const Transform* transform = entity->GetComponent<Transform>();
		const BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();

		const Point position = transform->position + boxCollider->offset;
		const Scale boxHalfSize = transform->scale * boxCollider->size * 0.5f;

		const Point localTL = { -boxHalfSize.width, -boxHalfSize.height };
		const Point localTR = { boxHalfSize.width, -boxHalfSize.height };
		const Point localBR = { boxHalfSize.width, boxHalfSize.height };
		const Point localBL = { -boxHalfSize.width, boxHalfSize.height };

		const Point rotateTL = Math::RotatePoint(localTL, -transform->angle);
		const Point rotateTR = Math::RotatePoint(localTR, -transform->angle);
		const Point rotateBR = Math::RotatePoint(localBR, -transform->angle);
		const Point rotateBL = Math::RotatePoint(localBL, -transform->angle);

		const std::array<SDL_FPoint, 5> rotatePoints
		{
			// 왼쪽 위
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotateTL.x,
				.y = getCameraOffset().y - position.y - rotateTL.y
			},

			// 오른쪽 위
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotateTR.x,
				.y = getCameraOffset().y - position.y - rotateTR.y
			},

			// 오른쪽 아래
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotateBR.x,
				.y = getCameraOffset().y - position.y - rotateBR.y
			},

			// 왼쪽 아래
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotateBL.x,
				.y = getCameraOffset().y - position.y - rotateBL.y
			},

			// 0번째
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotateTL.x,
				.y = getCameraOffset().y - position.y - rotateTL.y
			},
		};

		const DebugColor* debugColor = entity->GetComponent<DebugColor>();
		SDL_SetRenderDrawColor(mRenderer, debugColor->r, debugColor->g, debugColor->b, debugColor->a);
		SDL_RenderDrawLinesF(mRenderer, rotatePoints.data(), 5);
	}
}

void Core::labelRenderingSystem(const EntityWorld* entityWorld)
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
			.x = cameraCenter.x - mCameraTransform->position.x,
			.y = cameraCenter.y + mCameraTransform->position.y,
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

void Core::labelUIRenderingSystem(const EntityWorld* entityWorld)
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