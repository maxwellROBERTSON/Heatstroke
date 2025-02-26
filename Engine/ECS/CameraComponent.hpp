#pragma once

#include <string>

#include "Component.hpp"
#include "../../Game/Camera.hpp"

class CameraComponent : public Component<CameraComponent>
{
public:
	CameraComponent() {};

	// Getters
	Camera GetCamera() { return camera; }

	// Setters
	void SetCamera(Camera aCamera) { camera = aCamera; }

private:
	Camera camera;
};


