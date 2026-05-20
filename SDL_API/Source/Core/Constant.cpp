#include "pch.h"
#include "Constant.h"

const Constant& Constant::Get()
{
	static Constant constant{};
	return constant;
}

uint32_t Constant::GetWidth() const
{
	return 800;
}

uint32_t Constant::GetHeight() const
{
	return 600;
}

float Constant::GetHalfWidth() const
{
	return float((GetWidth() - 1) * 0.5f);
}

float Constant::GetHalfHeight() const
{
	return float((GetHeight() - 1) * 0.5f);
}