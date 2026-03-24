#pragma once

#include "Core/Entity/Entity.h"

#include "Core/Clip.h"
#include "Core/ComponentTypes.h"
#include "Core/Font.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

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
	Font mFont{};
	Entity mLabel{};

	Camera mMainCamera{};

	Entity mPlayer{};
	std::array<Texture, 5> mPlayerIdleTextures{};
	Clip mPlayerIdle{};

	Entity mMonster{};
	Texture mMonsterTexture{};
};