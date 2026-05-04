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
	bool active = false;
};

struct Label final : public Component
{
	static constexpr uint32_t _ID = 0;

	Label() : Component(&_ID) {}

	// TODO: Label에서 리소스를 들고 있기 때문에, 직접 해제한다.
	//~Label()
	//{
	//	if (surface != nullptr)
	//	{
	//		SDL_FreeSurface(surface);
	//	}

	//	if (texture != nullptr)
	//	{
	//		SDL_DestroyTexture(texture);
	//	}
	//}

	void SetText(Helper* helper, const std::string& text)
	{
		assert(helper != nullptr);

		if (surface != nullptr)
		{
			SDL_FreeSurface(surface);
		}

		if (texture != nullptr)
		{
			SDL_DestroyTexture(texture);
		}

		surface = TTF_RenderText_Blended(font->GetFont(), text.c_str(), color);
		assert(surface != nullptr);

		width = float(surface->w);
		height = float(surface->h);
		
		texture = SDL_CreateTextureFromSurface(helper->GetRenderer(), surface);
		assert(texture != nullptr);
	}

	Font* font = nullptr;
	SDL_Color color{ .a = 255 };
	SDL_Surface* surface = nullptr;
	SDL_Texture* texture = nullptr;

	float width = 0.0f;
	float height = 0.0f;

	bool active = false;
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
	float elapsedTime{}; // 흐른 시간
	uint32_t frameIndex{};
	bool active = false;
};