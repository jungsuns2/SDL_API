#pragma once
#include "Core/ComponentTypes.h"
#include "Core/Entity/Entity.h"
#include "Core/Font.h"
#include "Core/Label.h"
#include "Core/Scene.h"

class EntityWorld;

class MainScene final : public Scene
{
public:
	MainScene() = default;
	MainScene(const MainScene&) = delete;
	const MainScene& operator=(const MainScene&) = delete;
	~MainScene() = default;

public:
	void Initialize() override;
	bool Update(const float deltaTime) override;
	void Finalize()  override;

private:
	EntityWorld* mEntityWorld = nullptr;

private:
	Font mFont{};
	Label mLabel{};

	Entity mPlayer{};
	Texture mPlayerTexture{};
};