#include "Callbacks.hpp"

#include "Input.hpp"
#include "InputCodes.hpp"
#include "Joystick.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

#include <iostream>

#include "../Events/Event.hpp"
#include "../Events/KeyEvent.hpp"
#include "../Events/MouseEvent.hpp"
#include "../Events/WindowEvent.hpp"

	/*void registerCallbacks(GLFWwindow* aWindow, Engine::Game* game) {

		glfwSetWindowUserPointer(aWindow, game);

		glfwSetKeyCallback(aWindow, &onKeyPress);
		glfwSetMouseButtonCallback(aWindow, &onMouseButton);
		glfwSetCursorPosCallback(aWindow, &onMouseMove);
		glfwSetJoystickCallback(&joyStickCallback);*/
namespace Engine {
	void onWindowClose(GLFWwindow* aWindow)
	{
		VulkanWindow& engineWindow = *(VulkanWindow*)glfwGetWindowUserPointer(aWindow);
		Engine::WindowCloseEvent event;
		engineWindow.EventCallback(event);
	}

	void onKeyPress(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int aModifiers) {
		Engine::Game* game = static_cast<Engine::Game*>(glfwGetWindowUserPointer(aWindow));

		if (aAction == GLFW_PRESS)
		{
			switch (aKey) {
			case GLFW_KEY_ESCAPE:
				//glfwSetWindowShouldClose(aWindow, true);
				if (game->GetRenderMode() != GUIX)
					game->GetRenderMode() = GUIX;
				else
					game->GetRenderMode() = FORWARD;
				break;
			}
		}
	}
	void joyStickCallback(int jid, int event)
	{
		Joystick& joystick = static_cast<Joystick&>(InputManager::getJoystick(jid));
		if (event == GLFW_CONNECTED) {
			std::cout << joystick.getDeviceName() << " conneceted" << std::endl;
		}
		else {
			std::cout << joystick.getDeviceName() << " disconneceted" << std::endl;

		}
	}

	void onKeyPress(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int aModifiers)
	{
		VulkanWindow& engineWindow = *(VulkanWindow*)glfwGetWindowUserPointer(aWindow);
		if (aKey == HS_KEY_ESCAPE)
		{
			Engine::WindowCloseEvent event;
			engineWindow.EventCallback(event);
		}
		auto& keyboard = InputManager::getKeyboard();
		switch (aAction)
		{
		case GLFW_PRESS:
		{
			KeyPressedEvent event(aKey, 0);
			engineWindow.EventCallback(event);
			keyboard.setKey(aKey, ButtonState::PRESSED);
			break;
		}
		case GLFW_RELEASE:
		{
			KeyReleasedEvent event(aKey);
			engineWindow.EventCallback(event);
			keyboard.setKey(aKey, ButtonState::RELEASED);
			break;
		}
		}
	}

	void onMouseMove(GLFWwindow* aWindow, double x, double y)
	{
		auto& mouse = InputManager::getMouse();
		mouse.xPos = x;
		mouse.yPos = y;
	}
	void onMouseButton(GLFWwindow* aWindow, int aButton, int aAction, int aModifiers)
	{
		VulkanWindow& engineWindow = *(VulkanWindow*)glfwGetWindowUserPointer(aWindow);
		auto& mouse = InputManager::getMouse();

		switch (aAction) {
		case GLFW_PRESS:
		{
			mouse.mButtonStates[aButton] = ButtonState::PRESSED;
			MouseButtonPressedEvent event(aButton);
			engineWindow.EventCallback(event);
			break;
		}
		case GLFW_RELEASE:
		{
			mouse.mButtonStates[aButton] = ButtonState::RELEASED;
			MouseButtonReleasedEvent event(aButton);
			engineWindow.EventCallback(event);
			break;
		}
		}
	}
	void onMouseScroll(GLFWwindow* aWindow, double xOffset, double yOffset)
	{
		auto& mouse = InputManager::getMouse();
		// MouseScroll Event
		mouse.scrollPos = yOffset;
	}
}
