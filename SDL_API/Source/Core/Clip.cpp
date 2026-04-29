#include "pch.h"
#include "Clip.h"
#include "Texture.h"

void Clip::SetLoop(const bool loop)
{
	mLoop = loop;
}

bool Clip::IsLoop() const
{
	return mLoop;
}

void Clip::AddClip(const Frame& frame)
{
	assert(std::find(mFrames.begin(), mFrames.end(), frame) == mFrames.end());

	mFrames.push_back(frame);
}

const std::vector<Clip::Frame>& Clip::GetAllFrames() const
{
	return mFrames;
}

uint32_t Clip::GetLastFrameIndex() const
{
	return uint32_t(mFrames.size());
}