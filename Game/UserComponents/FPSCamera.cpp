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
}

//void FPSCamera::OnEvent(GLFWwindow* aWindow, Engine::Event& e)
//{
//}
