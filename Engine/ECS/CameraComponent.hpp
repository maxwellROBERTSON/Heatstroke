#pragma once

#include <string>

#include "Component.hpp"
#include "../Core/Camera.hpp"

class CameraComponent : public Component<CameraComponent>
{
public:
	CameraComponent() {};

	virtual void operator=(const CameraComponent& other) override
	{
		this->camera = other.camera;
	}

	// Getters
	Engine::Camera* GetCamera() { return &camera; }

	// Setters
	void SetCamera(Engine::Camera aCamera) { camera = aCamera; }

private:
	Engine::Camera camera;
};


