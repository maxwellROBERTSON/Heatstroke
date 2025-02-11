#include "Camera.hpp"

#include <glm/detail/func_trigonometric.inl>

#include "../Engine/glfw/Mouse.hpp"

Camera::Camera(float fov, float near, float far, glm::vec3 position, glm::vec3 frontDirection) : 
	fov(fov), nearPlane(near), farPlane(far), position(position), frontDirection(frontDirection) {}

void Camera::updateCamera() {

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