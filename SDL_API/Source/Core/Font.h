#pragma once

class Font final
{
public:
	Font() = default;
	Font(const Font&) = delete;
	const Font& operator=(const Font&) = delete;
	~Font() = default;

public:
	void Initilize(const std::string& filename, const uint32_t size);
	void Finalize();

public:
	[[nodiscard]] TTF_Font* GetFont() const;
	
private:
	TTF_Font* mFont = nullptr;
};

