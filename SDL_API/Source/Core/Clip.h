#pragma once

class Texture;

class Clip
{
public:
	struct Frame
	{
		bool operator==(const Frame& other) const
		{
			return texture == other.texture and durationTime == other.durationTime;
		}

		Texture* texture;
		float durationTime;
	};

public:
	Clip() = default;
	Clip(const Clip&) = delete;
	const Clip operator=(Clip&) = delete;
	~Clip() = default;

public:
	void SetLoop(const bool loop);
	[[nodiscard]] bool IsLoop()const;

	void AddClip(const Frame& frame);
	[[nodiscard]] const std::vector<Frame>& GetAllFrames() const;

	[[nodiscard]] uint32_t GetLastFrameIndex() const;

private:
	std::vector<Frame> mFrames{};
	bool mLoop = false;
	uint32_t mMaxFrame{};
};

