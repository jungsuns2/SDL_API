#include "pch.h"
#include "Input.h"

bool Input::GetKey(const SDL_Scancode virtualCode) const
{
	return mKeysPressed[virtualCode];
}

bool Input::GetKeyDown(const SDL_Scancode virtualCode) const
{
	return mKeysPressed[virtualCode] and mPrevKeysPressed[virtualCode];
}

bool Input::GetKeyUp(const SDL_Scancode virtualCode) const
{
	return not mKeysPressed[virtualCode] and mPrevKeysPressed[virtualCode];
}

bool Input::GetMouseButton(const Uint8 virtualMouse) const
{
	return mMousePressed[Uint8(virtualMouse)];
}

bool Input::GetMouseButtonDown(const Uint8 virtualMouse) const
{
	Uint8 index = Uint8(virtualMouse);
	return mMousePressed[index] and mPreMousePressed[index];
}

bool Input::GetMouseButtonUp(const Uint8 virtualMouse) const
{
	Uint8 index = Uint8(virtualMouse);
	return not mMousePressed[index] and mPreMousePressed[index];
}

SDL_FPoint Input::GetMousePosition() const
{
	return MousePosition;
}

void Input::SetMousePosition(SDL_FPoint mousePosition)
{
	MousePosition = mousePosition;
}

void Input::SetKeyState(const SDL_Scancode virtualCode, const bool bPressed)
{
	mPrevKeysPressed[virtualCode] = (mKeysPressed[virtualCode] != bPressed);
	mKeysPressed[virtualCode] = bPressed;
}

void Input::SetMouseButtonState(const Uint8 virtualMouse, const bool bPressed)
{
	Uint8 index = Uint8(virtualMouse);
	mPreMousePressed[index] = (mMousePressed[index] != bPressed);
	mMousePressed[index] = bPressed;
}

void Input::Clear()
{
	mPrevKeysPressed = {};
	mPreMousePressed = {};
}
