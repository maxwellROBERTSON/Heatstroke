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

	void Camera::init(GLFWwindow* aWindow)
	{
		if (this->firstClick)
		{
			int winX, winY;
			glfwGetFramebufferSize(aWindow, &winX, &winY);

			this->lastX = (float)winX / 2;
			this->lastY = (float)winY / 2;
			this->firstClick = false;
		}
	}

	void Camera::updateCamera(GLFWwindow* aWindow, float timeDelta, bool updatePosition) {
		if (glfwGetInputMode(aWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
			return;

		float deadzone = 0.05f;
		if (updatePosition)
		{
			float speedModifier = 1.0f;

			if (InputManager::IsPressed(HS_KEY_LEFT_SHIFT))
				speedModifier = 3.0f;

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
		}

		if (this->firstClick)
		{
			int winX, winY;
			glfwGetFramebufferSize(aWindow, &winX, &winY);

			this->lastX = (float)winX / 2;
			this->lastY = (float)winY / 2;
			this->firstClick = false;
		}

		switch (InputManager::getInputDevice()) {
		case InputDevice::CONTROLLER:
		{

			float xRot = 0.0f;
			float yRot = 0.0f;

			if (InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_X) > deadzone)
				xRot = InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_X);
			else if (InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_X) < -deadzone)
				xRot = InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_X);
			else
				xRot = 0.0f;

			if (InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_Y) > deadzone)
				yRot = InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_Y) * -1.0f;
			else if (InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_Y) < -deadzone)
				yRot = InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_Y) * -1.0f;
			else
				yRot = 0.0f;

			this->yaw += xRot * sensitivity;
			this->pitch += yRot * sensitivity;

			if (this->pitch > 89.9f)
				this->pitch = 89.9f;
			if (this->pitch < -89.9f)
				this->pitch = -89.9f;

			glm::vec3 newDir;
			newDir.x = std::cos(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
			newDir.y = std::sin(glm::radians(this->pitch));
			newDir.z = std::sin(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
			this->frontDirection = glm::normalize(newDir);
			break;
		}
		case InputDevice::KBM:
		{
			auto& mouse = InputManager::getMouse();

			float xOffset = mouse.getXPos() - this->lastX;
			float yOffset = this->lastY - mouse.getYPos();

			this->lastX = mouse.getXPos();
			this->lastY = mouse.getYPos();

			// Sensitivity multiplier
			xOffset *= 0.1f * sensitivity;
			yOffset *= 0.1f * sensitivity;

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
			break;
		}
		}
		//if (InputManager::hasJoysticksConnected())
		//{

		//	float xRot = 0.0f;
		//	float yRot = 0.0f;

		//	if (InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_X) > deadzone)
		//		xRot = InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_X);
		//	else if (InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_X) < -deadzone)
		//		xRot = InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_X);
		//	else
		//		xRot = 0.0f;

		//	if (InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_Y) > deadzone)
		//		yRot = InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_Y) * -1.0f;
		//	else if (InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_Y) < -deadzone)
		//		yRot = InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_Y) * -1.0f;
		//	else
		//		yRot = 0.0f;

		//	this->yaw += xRot * sensitivity;
		//	this->pitch += yRot * sensitivity;

		//	if (this->pitch > 89.9f)
		//		this->pitch = 89.9f;
		//	if (this->pitch < -89.9f)
		//		this->pitch = -89.9f;

		//	glm::vec3 newDir;
		//	newDir.x = std::cos(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
		//	newDir.y = std::sin(glm::radians(this->pitch));
		//	newDir.z = std::sin(glm::radians(this->yaw)) * std::cos(glm::radians(this->pitch));
		//	this->frontDirection = glm::normalize(newDir);
		//}
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
}

