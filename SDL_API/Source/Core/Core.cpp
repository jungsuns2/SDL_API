#include "pch.h"
#include "Core.h"

#include "Entity/Entity.h"
#include "Entity/EntityWorld.h"

#include "Clip.h"
#include "Collision.h"
#include "CollisionMath.h"

void Core::Initialize(Scene* scene)
{
	assert(scene != nullptr);

	SDL_Init(SDL_INIT_EVERYTHING);

	mWindow = SDL_CreateWindow("Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		Constant::Get().GetWidth(), Constant::Get().GetHeight(), SDL_WINDOW_SHOWN);

	// SDL_RENDERER_PRESENTVSYNC: 모니터 주사율에 맞춘다.
	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	mHelper.Initialize(mRenderer);
	
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	TTF_Init();

	ChangeScene(scene);
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

	removeEntitySystem();

	updateCollision(entityWorld);

	updateAnimator(entityWorld, deltaTime);
	drawImages(entityWorld);

#if defined(_DEBUG)
	colliderImageRenderingSystem(entityWorld);
#endif

	labelUIRenderingSystem(entityWorld);
	labelRenderingSystem(entityWorld);

	SDL_RenderPresent(mRenderer); // 화면에 출력한다.
	
	for (uint32_t i = 0; i < Constant::MAX_LAYER; ++i)
	{
		mLayers[i].clear();
	}

	return true;
}

void Core::Finalize()
{
	mScene->Finalize();
	delete (mScene);
	mScene = nullptr;

	delete mCameraTransform;
	mCameraTransform = nullptr;

	IMG_Quit();

	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);

	SDL_Quit();
}

void Core::ChangeScene(Scene* scene)
{
	assert(scene != nullptr);

	if (mScene != nullptr)
	{
		mScene->Finalize();
		delete (mScene);
		mScene = nullptr;	
	}

	mScene = scene;
	mScene->_SetHelper(&mHelper);
	mScene->SetType(mSceneType);
	mScene->Initialize();
}

void Core::SetSceneType(Scene::eSceneType type)
{
	mSceneType = type;
}

void Core::updateCollision(const EntityWorld* entityWorld)
{
	assert(entityWorld != nullptr);

	for (const Entity* entity0 : entityWorld->GetAllEntities())
	{
		if (not entity0->HasComponent<Transform>()
			or not entity0->HasComponent<CollisionDetector>())
		{
			continue;
		}

		CollisionDetector* collisionDetector0 = entity0->GetComponent<CollisionDetector>();
		if (collisionDetector0->CollisionLayerMask.none())
		{
			continue;
		}

		for (const Entity* entity1 : entityWorld->GetAllEntities())
		{
			if (not entity1->HasComponent<Transform>()
				or not entity1->HasComponent<CollisionDetector>())
			{
				continue;
			}

			if (entity0 == entity1)
			{
				continue;
			}

			CollisionDetector* collisionDetector1 = entity1->GetComponent<CollisionDetector>();
			if (not collisionDetector0->CollisionLayerMask[collisionDetector1->Layer])
			{
				continue;
			}

			if (entity0->HasComponent<Active>()
				or entity1->HasComponent<Active>())
			{
				const Active* active0 = entity0->GetComponent<Active>();
				const Active* active1 = entity1->GetComponent<Active>();
				if (not active0->isValue
					or not active1->isValue)
				{
					continue;
				}
			}

			Collision::Get().CheckCollisionBoxBox(*entity0, *entity1);
		}
	}
}

void Core::updateAnimator(const EntityWorld* entityWorld, const float deltaTime)
{
	assert(entityWorld != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntities())
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

	for (const Entity* entity : entityWorld->GetAllEntities())
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

	for (const Entity* entity : entityWorld->GetAllEntities())
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
		
		Color color{};
		if (entity->HasComponent<Color>())
		{
			color = *entity->GetComponent<Color>();
		}

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

		LayerDesc desc =
		{
			.transform = transform,
			.texture = texture,
			.color = color,
			.rect = rect,
			.angleCenter = angleCenter
		};

		assert(image->layer < Constant::MAX_LAYER and "최대 Layer 개수를 초과했습니다.");
		mLayers[image->layer].push_back(desc);
	}

	for (uint32_t i = 0; i < Constant::MAX_LAYER; ++i)
	{
		for (const auto& layer : mLayers[i])
		{
			SDL_SetTextureColorMod(layer.texture->GetTexture(), layer.color.r, layer.color.g, layer.color.b);
			SDL_SetTextureAlphaMod(layer.texture->GetTexture(), layer.color.a);
			SDL_RenderCopyExF(mRenderer, layer.texture->GetTexture(), nullptr, &layer.rect, layer.transform->angle, &layer.angleCenter, layer.transform->flip);
		}
	}
}

void Core::colliderImageRenderingSystem(const EntityWorld* entityWorld)
{
	assert(entityWorld != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntities())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<CollisionDetector>()
			or not entity->HasComponent<DebugActive>()
			or not entity->HasComponent<DebugColor>())
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

		if (const DebugActive* active = entity->GetComponent<DebugActive>();
			not active->isValue)
		{
			continue;
		}

		const Transform* transform = entity->GetComponent<Transform>();
		const BoxCollider* boxCollider = entity->GetComponent<BoxCollider>();
		const Direction* direction = entity->GetComponent<Direction>();
		
		Point position = { .x = 0.0f, .y = transform->position.y + boxCollider->offset.y };
		position.x = (direction->value.x > 0.0f) ?
			transform->position.x + boxCollider->offset.x : transform->position.x - boxCollider->offset.x;

		const Scale boxHalfSize = transform->scale * boxCollider->size * 0.5f;

		const Quad local =
		{
			.leftTop = { -boxHalfSize.width, -boxHalfSize.height },
			.rightTop = { boxHalfSize.width, -boxHalfSize.height },
			.leftBottom = { -boxHalfSize.width, boxHalfSize.height },
			.rightBottom = { boxHalfSize.width, boxHalfSize.height }
		};

		const Quad rotate =
		{
			.leftTop = Math::RotatePoint(local.leftTop, -transform->angle),
			.rightTop = Math::RotatePoint(local.rightTop, -transform->angle),
			.leftBottom = Math::RotatePoint(local.leftBottom, -transform->angle),
			.rightBottom = Math::RotatePoint(local.rightBottom, -transform->angle)
		};

		const std::array<SDL_FPoint, 5> result
		{ 
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotate.leftTop.x,
				.y = getCameraOffset().y - position.y - rotate.leftTop.y
			},
			
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotate.rightTop.x,
				.y = getCameraOffset().y - position.y - rotate.rightTop.y
			},
			
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotate.rightBottom.x,
				.y = getCameraOffset().y - position.y - rotate.rightBottom.y
			},
			
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotate.leftBottom.x,
				.y = getCameraOffset().y - position.y - rotate.leftBottom.y
			},
			
			SDL_FPoint
			{
				.x = getCameraOffset().x + position.x + rotate.leftTop.x,
				.y = getCameraOffset().y - position.y - rotate.leftTop.y
			}
		};

		const DebugColor* debugColor = entity->GetComponent<DebugColor>();
		SDL_SetRenderDrawColor(mRenderer, debugColor->r, debugColor->g, debugColor->b, debugColor->a);
		SDL_RenderDrawLinesF(mRenderer, result.data(), int(result.size()));
	}
}

void Core::labelRenderingSystem(const EntityWorld* entityWorld)
{
	assert(entityWorld != nullptr);

	for (const Entity* entity : entityWorld->GetAllEntities())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<Label>()
			or entity->HasComponent<Ui>())
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

	for (const Entity* entity : entityWorld->GetAllEntities())
	{
		if (not entity->HasComponent<Transform>()
			or not entity->HasComponent<Label>()
			or not entity->HasComponent<Ui>())
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

void Core::removeEntitySystem()
{
	EntityWorld* entityWorld = mScene->GetEntityWorld();
	entityWorld->CleanRemovedEntities();
}

SDL_Window* Core::GetWindow() const
{
	return mWindow;
}