#include "Camera.hpp"

#include <GLFW/glfw3.h>

#include "../Events/Event.hpp"
#include "../Events/KeyEvent.hpp"
#include "../Events/MouseEvent.hpp"
#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Input/Keyboard.hpp"
#include "../Input/Mouse.hpp"
#include <iostream>

#include <GLFW/glfw3.h>

/*
* This entire class will probably need reworking, its not the best implementation of a first person camera
* but is one that somewhat works that isn't the janky one from Advanced Rendering's Assignment 2.
*/

namespace Engine
{
	Camera::Camera(float fov, float _near, float _far, glm::vec3 position, glm::vec3 frontDirection) :
		fov(fov), nearPlane(_near), farPlane(_far), position(position), frontDirection(frontDirection)
	{
	}

	void Camera::updateCamera(GLFWwindow* aWindow, float timeDelta) {
		if (glfwGetInputMode(aWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
			return;

		if (this->firstClick) {
			int winX, winY;
			glfwGetFramebufferSize(aWindow, &winX, &winY);

			this->lastX = (float)winX / 2;
			this->lastY = (float)winY / 2;
			this->firstClick = false;
		}

		switch (camMode) {
		case CameraMode::SCENE:
			updateSceneCamera(timeDelta);
			break;
		case CameraMode::PLAYER:
			updatePlayerCamera(timeDelta);
			break;
		default:
			break;
		}

	}

	void Camera::OnEvent(GLFWwindow* aWindow, Engine::Event& e)
	{
		Engine::EventDispatcher dispatcher(e);

		dispatcher.Dispatch<MouseButtonPressedEvent>(
			[&](MouseButtonPressedEvent& event)
			{
				if (event.GetMouseButton() == HS_MOUSE_BUTTON_RIGHT)
				{
					if (glfwGetInputMode(aWindow, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
						glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					}
					else if (glfwGetInputMode(aWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
						glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					}
					return true;
				}
				return false;
			}
		);
	}
	void Camera::updateSceneCamera(float timeDelta)
	{
		auto& mouse = InputManager::getMouse();
		float speedModifier = 1.0f;
		if (InputManager::IsPressed(HS_KEY_LEFT_SHIFT)) speedModifier = 3.0f;
		float distance = 1.0f * speedModifier * timeDelta;

		if (InputManager::IsPressed(HS_KEY_W))
		{
			this->position += distance * this->frontDirection;
		}
		if (InputManager::IsPressed(HS_KEY_S))
		{
			this->position -= distance * this->frontDirection;
		}
		if (InputManager::IsPressed(HS_KEY_D))
		{
			this->position += glm::normalize(glm::cross(this->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * distance;
		}
		if (InputManager::IsPressed(HS_KEY_A))
		{
			this->position -= glm::normalize(glm::cross(this->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * distance;
		}
		if (InputManager::IsPressed(HS_KEY_E))
		{
			this->position += distance * glm::vec3(0.0f, 1.0f, 0.0f);
		}
		if (InputManager::IsPressed(HS_KEY_Q))
		{
			this->position -= distance * glm::vec3(0.0f, 1.0f, 0.0f);
		}
		//std::fprintf(stdout, "Pos: %f %f %f\n", this->position.x, this->position.y, this->position.z);



		float xOffset = mouse.getXPos() - this->lastX;
		float yOffset = this->lastY - mouse.getYPos();

		this->lastX = mouse.getXPos();
		this->lastY = mouse.getYPos();

		xOffset *= 0.1f; // Sensitivity multiplier
		yOffset *= 0.1f;

		this->yaw += xOffset;
		this->pitch += yOffset;

		if (this->pitch > 89.9f)
			this->pitch = 89.9f;
		if (this->pitch < -89.9f)
			this->pitch = -89.9f;

		glm::vec3 newDir;
		newDir.x = std::cos(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
		newDir.y = std::sin(glm::radians(this->pitch));
		newDir.z = std::sin(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
		this->frontDirection = glm::normalize(newDir);
	}
	void Camera::updatePlayerCamera(float timeDelta)
	{

		auto& mouse = InputManager::getMouse();
		float speedModifier = 1.0f;
		if (InputManager::IsPressed(HS_KEY_LEFT_SHIFT)) speedModifier = 3.0f;
		float distance = 1.0f * speedModifier * timeDelta;

		//if (InputManager::IsPressed(HS_KEY_W))
		//{
		//	this->position += distance * this->frontDirection;
		//}
		//if (InputManager::IsPressed(HS_KEY_S))
		//{
		//	this->position -= distance * this->frontDirection;
		//}
		//if (InputManager::IsPressed(HS_KEY_D))
		//{
		//	this->position += glm::normalize(glm::cross(this->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * distance;
		//}
		//if (InputManager::IsPressed(HS_KEY_A))
		//{
		//	this->position -= glm::normalize(glm::cross(this->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * distance;
		//}

		float xOffset = mouse.getXPos() - this->lastX;
		float yOffset = this->lastY - mouse.getYPos();

		this->lastX = mouse.getXPos();
		this->lastY = mouse.getYPos();

		xOffset *= 0.1f; // Sensitivity multiplier
		yOffset *= 0.1f;

		this->yaw += xOffset;
		this->pitch += yOffset;

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

