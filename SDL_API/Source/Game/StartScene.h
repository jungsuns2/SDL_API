#pragma once
#include "Core/Scene.h"
#include "Core/Texture.h"

class StartScene final : public Scene
{
public:
	enum class CollisionLayer
	{
		Button,
		MouseCursor,
		Count
	};

	enum class Layer
	{
		BackGround,
		Logo,
		Button,
		Count
	};

public:
	StartScene() = default;
	StartScene(const StartScene&) = delete;
	const StartScene& operator=(const StartScene&) = delete;
	~StartScene() = default;

public:
	void Initialize() override;
	bool Update(const float deltaTime) override;
	void Finalize()  override;

private:
	template <typename T>
	Entity* getEntity() const;

	template <typename T>
	std::vector<Entity*> getEntities() const;

	Point getScreenMousePosition() const;

	void quitEvent();

public:
	bool mIsUpdate = true;
	bool mIsDebugActive = false;

	Texture mBgSkyTexture{};
	Texture mLogoTexture{};

	Texture mStartButtonNormalTexture{};
	Texture mStartButtonHoverTexture{};

	Texture mExitButtonNormalTexture{};
	Texture mExitButtonHoverTexture{};
};

