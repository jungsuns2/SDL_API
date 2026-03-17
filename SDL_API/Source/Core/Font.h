#pragma once

class Label;

class Font final
{
public:
	Font() = default;
	Font(const Font&) = delete;
	const Font& operator=(const Font&) = delete;
	~Font() = default;

public:
	void Initilize(const std::string& filename);
	void Finalize();

public:
	TTF_Font* GetFont() const;

protected:
	void SetSize(const uint32_t size);

private:
	TTF_Font* mFont = nullptr;
	uint32_t mSize = 30;

	friend Label;
};

