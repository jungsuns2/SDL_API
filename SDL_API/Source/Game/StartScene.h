#pragma once
#include "Core/Scene.h"
#include "Core/Texture.h"

class StartScene final : public Scene
{
public:
	StartScene() = default;
	StartScene(const StartScene&) = delete;
	const StartScene& operator=(const StartScene&) = delete;
	~StartScene() = default;

public:
	void Initialize() override;
	bool Update(const float deltaTime) override;
	void Finalize()  override;

public:
	bool mIsUpdate = true;

	Texture mBgSkyTexture{};
	Texture mLogoTexture{};
};

