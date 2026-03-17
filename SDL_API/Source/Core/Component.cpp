#include "pch.h"
#include "Component.h"

Component::Component(const uint32_t id)
	: mID(id)
{

}

uint32_t Component::GetID() const
{
	return mID;
}
