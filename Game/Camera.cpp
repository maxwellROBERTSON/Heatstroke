#include "Camera.hpp"

#include <GLFW/glfw3.h>
#include <glm/detail/func_trigonometric.inl>

#include "../Engine/Input/Keyboard.hpp"
#include "../Engine/Input/Mouse.hpp"


/*
* This entire class will probably need reworking, its not the best implementation of a first person camera
* but is one that somewhat works that isn't the janky one from Advanced Rendering's Assignment 2.
*/
Camera::Camera(float fov, float near, float far, glm::vec3 position, glm::vec3 frontDirection) :
	fov(fov), nearPlane(near), farPlane(far), position(position), frontDirection(frontDirection) {
}

void Camera::updateCamera(GLFWwindow* aWindow, float timeDelta) {
	if (glfwGetInputMode(aWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
		return;

	for (const auto& [key, state] : Engine::Keyboard::getKeyStates()) {
		if (state.first) { // Key was pressed down

			float speedModifier = 1.0f;
			if (state.second == GLFW_MOD_SHIFT) speedModifier = 3.0f;
			float distance = 1.0f * speedModifier * timeDelta;

			switch (key) {
			case GLFW_KEY_W:
				this->position += distance * this->frontDirection;
				break;
			case GLFW_KEY_S:
				this->position -= distance * this->frontDirection;
				break;
			case GLFW_KEY_D:
				this->position += glm::normalize(glm::cross(this->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * distance;
				break;
			case GLFW_KEY_A:
				this->position -= glm::normalize(glm::cross(this->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * distance;
				break;
			case GLFW_KEY_E:
				this->position += distance * glm::vec3(0.0f, 1.0f, 0.0f);
				break;
			case GLFW_KEY_Q:
				this->position -= distance * glm::vec3(0.0f, 1.0f, 0.0f);
				break;
			}
		}
	}

	//std::fprintf(stdout, "Pos: %f %f %f\n", this->position.x, this->position.y, this->position.z);

	if (this->firstClick) {
		int winX, winY;
		glfwGetFramebufferSize(aWindow, &winX, &winY);

		this->lastX = (float)winX / 2;
		this->lastY = (float)winY / 2;
		this->firstClick = false;
	}

	float xOffset = Engine::Mouse::getX() - this->lastX;
	float yOffset = this->lastY - Engine::Mouse::getY();

	this->lastX = Engine::Mouse::getX();
	this->lastY = Engine::Mouse::getY();

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

void Camera::OnEvent(Engine::Event& e)
{
}
