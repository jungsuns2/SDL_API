#include "pch.h"
#include "Entity.h"

Entity::~Entity()
{
	for (Component* component : mComponents)
	{
		if (component != nullptr)
		{
			delete component;
			component = nullptr;
		}
	}
}

#if defined(_DEBUG)
void Entity::SetSourceLocation(const char* filename, const uint32_t line)
{
	assert(filename != nullptr);

	mSourceLocation = strstr(filename, "Source");
	mSourceLocation += ":";
	mSourceLocation += std::to_string(line);
}
#endif
