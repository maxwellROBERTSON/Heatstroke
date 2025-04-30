#include "FPSCamera.hpp"

#include "../Events/Event.hpp"
#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Input/Keyboard.hpp"
#include "../Input/Mouse.hpp"

void FPSCamera::updateCamera(GLFWwindow* aWindow, float timeDelta)
{
	if (glfwGetInputMode(aWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
		return;

	float speedModifier = 1.5f;
	float distance = 1.0f * speedModifier * timeDelta;

	auto& mouse = Engine::InputManager::getMouse();

	if (Engine::InputManager::IsPressed(HS_KEY_W))
	{
		this->position += distance * this->frontDirection;
	}
	if (Engine::InputManager::IsPressed(HS_KEY_S))
	{
		this->position -= distance * this->frontDirection;
	}
	if (Engine::InputManager::IsPressed(HS_KEY_D))
	{
		this->position += glm::normalize(glm::cross(this->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * distance;
	}
	if (Engine::InputManager::IsPressed(HS_KEY_A))
	{
		this->position -= glm::normalize(glm::cross(this->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * distance;
	}

	if (enableRotation)
	{
		float xOffset = mouse.getXPos() - this->lastX;
		float yOffset = this->lastY - mouse.getYPos();

		this->lastX = mouse.getXPos();
		this->lastY = mouse.getYPos();

		xOffset *= 0.1f; // Sensitivity multiplier
		yOffset *= 0.1f;

		this->yaw += xOffset;
		this->pitch += yOffset;

		//if (this->pitch > 89.9f)
		//	this->pitch = 89.9f;
		//if (this->pitch < -89.9f)
		//	this->pitch = -89.9f;

		if (this->pitch > 59.9f)
			this->pitch = 59.9f;
		if (this->pitch < -59.9f)
			this->pitch = -59.9f;


		glm::vec3 newDir;
		newDir.x = std::cos(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
		newDir.y = std::sin(glm::radians(this->pitch));
		newDir.z = std::sin(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
		this->frontDirection = glm::normalize(newDir);
	}
}

//void FPSCamera::SetCameraOffset(float x, float y, float z)
//{
//	cameraOffset = glm::vec3(x, y, z);
//}
//
//void FPSCamera::SetCameraOffset(glm::vec3 offset)
//{
//	cameraOffset = offset;
//}

//void FPSCamera::OnEvent(GLFWwindow* aWindow, Engine::Event& e)
//{
//}
