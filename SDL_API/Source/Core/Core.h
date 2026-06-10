#pragma once
#include "ComponentTypes.h"

#include "Helper.h"
#include "Scene.h"
#include "Texture.h"

struct TextureSystemDesc
{
	const Scale textureScale;
	const Transform* textureTransform;
	SDL_FRect* rect;
	SDL_FPoint* angleCenter;
};

struct DrawSystemDesc
{
	const Scale textureScale;
	Transform* transform;
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
	Point getCameraOffset() const;

	void textureSystem(const TextureSystemDesc& desc);
	void drawSystem(const DrawSystemDesc& desc);
	void drawImages(const EntityWorld* entityWorld);

	void colliderImageRenderingSystem(const EntityWorld* entityWorld);
	void labelRenderingSystem(const EntityWorld* entityWorld);
	void labelUIRenderingSystem(const EntityWorld* entityWorld);

	void removeEntitySystem();

	[[nodiscard]] SDL_Window* GetWindow() const;

private:
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;

	Helper mHelper{};
	Scene* mScene = nullptr;

	Transform* mCameraTransform = nullptr;

	Texture mColliderTexture{};
};