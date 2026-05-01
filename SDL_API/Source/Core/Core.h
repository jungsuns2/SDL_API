#pragma once
#include "Scene.h"
#include "Helper.h"

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

	void ImageSystem(const Entity* entity, const Point cameraPosition);
	void animatorSystem(const EntityWorld* entityWorld, const Point cameraOffset, const float deltaTime);

	[[nodiscard]] Point getCenterOffset() const;

public:
	[[nodiscard]] SDL_Window* GetWindow() const;

private:
	SDL_Window* mWindow = nullptr;
	SDL_Renderer* mRenderer = nullptr;

	Helper mHelper{};
	Scene* mScene = nullptr;
};