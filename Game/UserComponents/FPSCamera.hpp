#pragma once

#include "../Core/Camera.hpp"
#include <iostream>
#include <string>

#include <GLFW/glfw3.h>

class FPSCamera : public Engine::Camera
{
public:
	FPSCamera() : Engine::Camera() {}
	FPSCamera(const std::string& name, float fov, float n, float f, glm::vec3 position, glm::vec3 frontDirection) : Engine::Camera(fov, n, f, position, frontDirection)
	{
		mName = name;
	}

	virtual void updateCamera(GLFWwindow* aWindow, float timeDelta) override;
	//virtual void OnEvent(GLFWwindow* aWindow, Engine::Event& e) override;

	~FPSCamera() {}

	std::string mName;
};

