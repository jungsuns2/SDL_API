#pragma once
class Scene
{
public:
	Scene() = default;
	Scene(const Scene&) = delete;
	const Scene& operator=(Scene&) = delete;
	virtual ~Scene() = default;

public:
	virtual void Initialize() = 0;
	virtual bool Update() = 0;
	virtual void Finalize() = 0;
};

