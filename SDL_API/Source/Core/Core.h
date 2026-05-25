#pragma once
#include "ComponentTypes.h"

#include "Helper.h"
#include "Scene.h"
#include "Texture.h"

struct TextureSystemDesc
{
	const Scale textureScale;
	const Transform* textureTransform;
	const Transform* cameraTransform;
	SDL_FRect* rect;
	SDL_FPoint* angleCenter;
};

struct DrawSystemDesc
{
	const Scale textureScale;
	Transform* transform;
	Transform* cameraTransform;
	SDL_FRect* rect;
};

class EntityWorld;

class Core final
{
public:
	Core() = default;
	Core(const Core&) = delete;
	const Core& operator=(Core&) = delete;
	~Core() = default;

public:
	void Initialize(Scene* scene);
	bool Update(const float deltaTime);
	void Finalize();

private:
	void updateAnimator(const EntityWorld* entityWorld, const float deltaTime);
	Transform* cameraSystem(const EntityWorld* entityWorld);
	void textureSystem(const TextureSystemDesc& desc);
	void drawSystem(const DrawSystemDesc& desc);
	void drawImages(const EntityWorld* entityWorld, const Transform* cameraTransform);
	void colliderImageRenderingSystem(const EntityWorld* entityWorld, Transform* cameraTransform);
	void labelRenderingSystem(const EntityWorld* entityWorld, Transform* cameraTransform);
	void labelUIRenderingSystem(const EntityWorld* entityWorld, Transform* cameraTransform);

	[[nodiscard]] SDL_Window* GetWindow() const;

private:
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;

	Helper mHelper{};
	Scene* mScene = nullptr;

	Texture mColliderTexture{};
};