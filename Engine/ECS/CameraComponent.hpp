#pragma once

#include <string>

#include "Component.hpp"
#include "../../Game/Camera.hpp"

class CameraComponent : public Component<CameraComponent>
{
public:
	CameraComponent() {};

	virtual void operator=(const CameraComponent& other) override
	{
		this->camera = other.camera;
	}

	// Getters
	Camera GetCamera() { return camera; }

	// Setters
	void SetCamera(Camera aCamera) { camera = aCamera; }

private:
	Camera camera;
};


