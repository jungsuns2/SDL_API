#pragma once
#include "Core/Entity/Component.h"

struct BackGroundTag : public Component
{
	static constexpr uint32_t _ID = 0;
	BackGroundTag() : Component(&_ID) {}
};

struct LogoTag : public Component
{
	static constexpr uint32_t _ID = 0;
	LogoTag() : Component(&_ID) {}
};

struct StartTag : public Component
{
	static constexpr uint32_t _ID = 0;
	StartTag() : Component(&_ID) {}
};

struct ExitTag : public Component
{
	static constexpr uint32_t _ID = 0;
	ExitTag() : Component(&_ID) {}
};

struct MouseCursorTag : public Component
{
	static constexpr uint32_t _ID = 0;
	MouseCursorTag() : Component(&_ID) {}
};

struct Button : public Component
{
	static constexpr uint32_t _ID = 0;
	Button() : Component(&_ID) {}
};