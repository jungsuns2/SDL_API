#include "pch.h"
#include "Clip.h"
#include "Texture.h"

void Clip::SetLoop(const bool loop)
{
	mLoop = loop;
}

bool Clip::GetLoop() const
{
	return mLoop;
}

void Clip::AddClip(const Frame& frame)
{
	assert(std::find(mFrames.begin(), mFrames.end(), frame) == mFrames.end());

	mFrames.push_back(frame);
}

std::vector<Clip::Frame>& Clip::GetAllFrames()
{
	return mFrames;
}