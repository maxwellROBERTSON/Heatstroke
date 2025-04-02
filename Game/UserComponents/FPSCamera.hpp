#pragma once

#include "../Core/Camera.hpp"

class FPSCamera : public Engine::Camera
{
public:
	FPSCamera() : Camera() {}
	FPSCamera(float fov, float n, float f, glm::vec3 position, glm::vec3 frontDirection) : Camera(fov, n, f, position, frontDirection) {}

	virtual void updateCamera(GLFWwindow* aWindow, float timeDelta) override;
	//virtual void OnEvent(GLFWwindow* aWindow, Engine::Event& e) override;

	~FPSCamera() {}
};

