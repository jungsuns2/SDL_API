#pragma once

class Input final
{
public:
	Input() = default;
	Input(const Input&) = delete;
	const Input& operator=(const Input&) = delete;
	~Input() = default;

public:
	bool GetKey(const SDL_Scancode virtualCode) const;
	bool GetKeyDown(const SDL_Scancode virtualCode) const;
	bool GetKeyUp(const SDL_Scancode virtualCode) const;

	bool GetMouseButton(const Uint8 virtualMouse) const;
	bool GetMouseButtonDown(const Uint8 virtualMouse) const;
	bool GetMouseButtonUp(const Uint8 virtualMouse) const;

	SDL_FPoint GetMousePosition() const;
	void SetMousePosition(SDL_FPoint mousePosition);

public:
	void SetKeyState(const SDL_Scancode virtualCode, const bool bPressed);
	void SetMouseButtonState(const Uint8 virtualMouse, const bool bPressed);
	void Clear();

private:
	static constexpr size_t SDL_MOUSE_COUNT = 6;

	std::array<bool, SDL_NUM_SCANCODES> mKeysPressed{};
	std::array<bool, SDL_MOUSE_COUNT> mMousePressed{};

	std::array<bool, SDL_NUM_SCANCODES> mPrevKeysPressed{};
	std::array<bool, SDL_MOUSE_COUNT> mPreMousePressed{};

	SDL_FPoint MousePosition{};
};