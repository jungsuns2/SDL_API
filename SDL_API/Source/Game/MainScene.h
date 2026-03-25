#pragma once

#include "Core/Entity/Entity.h"

#include "Core/Clip.h"
#include "Core/ComponentTypes.h"
#include "Core/Font.h"
#include "Core/Scene.h"
#include "Core/Texture.h"

enum class PlayerState
{
	Idle,
	Run,
	Count
};

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

	Camera mMainCamera{};

	Entity mLabel{};
	Entity mPlayer{};
	Entity mMonster{};

	PlayerState mPlayerState{};
	std::array<Clip, uint32_t(PlayerState::Count)> mPlayerClips{};

	static constexpr uint32_t IDLE_COUNT = 5;
	static constexpr uint32_t RUN_COUNT = 6;

	std::array<Texture, IDLE_COUNT> mPlayerIdleTextures{};
	std::array<Texture, RUN_COUNT> mPlayerRunTextures{};
	Texture mMonsterTexture{};

};