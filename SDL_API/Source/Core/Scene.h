#pragma once
class Scene
{
public:
	Scene() = default;
	Scene(const Scene&) = delete;
	Scene& operator=(Scene&) = delete;
	~Scene() = default;

public:
	void Initialize();
	bool Update();
	void Finalize();
};

