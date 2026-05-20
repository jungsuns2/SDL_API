#pragma once
#include "Entity/Component.h"
#include "Entity/Entity.h"

#include "Font.h"
#include "Helper.h"

class Clip;
class Texture;

struct Transform final : public Component
{
	static constexpr uint32_t _ID = 0;
	Transform() : Component(&_ID) {}

	Point position{};
	Scale scale{ .width = 1.0f, .height = 1.0f };
	float angle = 0.0f;
	Point center{};
	SDL_RendererFlip flip{};
};

struct Image final : public Component
{
	static constexpr uint32_t _ID = 0;
	Image() : Component(&_ID) {}

	Texture* texture = nullptr;
};

struct Label final : public Component
{
	static constexpr uint32_t _ID = 0;
	Label() : Component(&_ID) {}

	~Label()
	{
		if (texture != nullptr)
		{
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}
	}

	Font* font = nullptr;
	SDL_Texture* texture = nullptr;
	std::string text{};
};

struct Camera final : public Component
{
	static constexpr uint32_t _ID = 0;
	Camera() : Component(&_ID) {}
};

struct Animator final : public Component
{
	static constexpr uint32_t _ID = 0;
	Animator() : Component(&_ID) {}

	void SetClip(Clip* clip)
	{
		assert(clip != nullptr);

		if (clipState == clip)
		{
			return;
		}

		clipState = clip;
		frameIndex = 0;
		elapsedTime = 0;
	}

	Clip* clipState = nullptr;
	float elapsedTime{}; // »Â∏• Ω√∞£
	uint32_t frameIndex{};
};

struct Color final : public Component
{
	static constexpr uint32_t _ID = 0;
	Color() : Component(&_ID) {}
	
	Uint8 r = 255;
	Uint8 g = 255;
	Uint8 b = 255;
	Uint8 a = 255;
};

struct DebugColor final : public Component
{
	static constexpr uint32_t _ID = 0;
	DebugColor() : Component(&_ID) {}

	Uint8 r = 0;
	Uint8 g = 200;
	Uint8 b = 0;
	Uint8 a = 100;
};

struct Direction final : public Component
{
	static constexpr uint32_t _ID = 0;
	Direction() : Component(&_ID) {}

	Point value{};
};

//struct Collider final : public Component
//{
//	static constexpr uint32_t _ID = 0;
//	Collider(const uint32_t layer) : Component(&_ID), Layer(layer) {}
//
//	const uint32_t Layer = 0;
//	std::bitset<64> CollisionLayerMask{};
//};

struct Active final : public Component
{
	static constexpr uint32_t _ID = 0;
	Active() : Component(&_ID) {}

	bool isValue = false;
};

struct DebugActive final : public Component
{
	static constexpr uint32_t _ID = 0;
	DebugActive() : Component(&_ID) {}

	bool value = false;
};

struct UI : public Component
{
	static constexpr uint32_t _ID = 0;
	UI() : Component(&_ID) {}
};