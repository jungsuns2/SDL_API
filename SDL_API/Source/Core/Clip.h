#pragma once

class Texture;
class Helper;

struct ClipInitDesc
{
	Helper* helper;
	const std::string& filename;
	const uint32_t maxSize;
	const float durationTime;
	const bool loop;
};

class Clip
{
public:
	struct Frame
	{
		Texture* texture;
		float durationTime;

		bool operator==(const Frame& other) const
		{
			return texture == other.texture and durationTime == other.durationTime;
		}
	};

public:
	Clip() = default;
	Clip(const Clip&) = delete;
	const Clip operator=(Clip&) = delete;
	~Clip() = default;

public:
	void SetLoop(const bool loop);
	[[nodiscard]] bool GetLoop()const;

	void AddClip(const Frame& frame);
	std::vector<Frame>& GetAllFrames();

private:
	std::vector<Frame> mFrames{};
	bool mLoop = false;
	uint32_t mMaxFrame{};
};

