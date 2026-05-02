#pragma once
#include "ComponentTypes.h"
#include "Helper.h"
#include "Scene.h"

struct TextureSystemDesc
{
	const Scale textureScale;
	Transform* textureTransform;
	Transform* cameraTransform;
	SDL_FRect* rect;
	SDL_FPoint* angleCenter;
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

	Transform* cameraSystem(const EntityWorld* entityWorld);
	void textureSystem(const TextureSystemDesc& desc);
	void imageSystem(const EntityWorld* entityWorld, Transform* cameraTransform);
	void animatorSystem(const EntityWorld* entityWorld, Transform* cameraTransform, const float deltaTime);
	void colliderSystem(const EntityWorld* entityWorld, Transform* cameraTransform);
	void labelSystem(const EntityWorld* entityWorld);

public:
	[[nodiscard]] SDL_Window* GetWindow() const;

private:
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;

	Helper mHelper{};
	Scene* mScene = nullptr;
};