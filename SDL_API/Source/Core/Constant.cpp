#include "pch.h"
#include "Constant.h"

const Constant& Constant::Get()
{
	static Constant constant{};
	return constant;
}

uint32_t Constant::GetWidth() const
{
	return 1080;
}

uint32_t Constant::GetHeight() const
{
	return 720;
}