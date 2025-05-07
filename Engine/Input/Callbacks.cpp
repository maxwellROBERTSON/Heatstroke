#include "Callbacks.hpp"

#include <iostream>

#include "../Events/Event.hpp"
#include "../Events/KeyEvent.hpp"
#include "../Events/MouseEvent.hpp"
#include "../Events/WindowEvent.hpp"
#include "Input.hpp"
#include "InputCodes.hpp"
#include "InputTypes.hpp"
#include "Joystick.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>

namespace Engine {
	void onWindowClose(GLFWwindow* aWindow)
	{
		VulkanWindow& engineWindow = *(VulkanWindow*)glfwGetWindowUserPointer(aWindow);
		Engine::WindowCloseEvent event;
		engineWindow.EventCallback(event);
	}

	void joyStickCallback(int jid, int event)
	{
		Joystick& joystick = static_cast<Joystick&>(InputManager::getJoystick(jid));
		if (event == GLFW_CONNECTED && glfwJoystickIsGamepad(jid)) {
			std::cout << joystick.getDeviceName() << " conneceted" << std::endl;
			InputManager::addJoysitck(jid);
		}
		else {
			std::cout << joystick.getDeviceName() << " disconneceted" << std::endl;
			InputManager::removeJoystick(jid);

		}
	}

	void onKeyPress(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int aModifiers)
	{
		VulkanWindow& engineWindow = *(VulkanWindow*)glfwGetWindowUserPointer(aWindow);
		auto& keyboard = InputManager::getKeyboard();

		if (aAction == GLFW_PRESS && aKey == HS_KEY_ESCAPE)
		{
			ESCEvent event;
			engineWindow.EventCallback(event);
			return;
		}

		if (!ImGui::GetIO().WantCaptureKeyboard)
		{
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
			case GLFW_REPEAT:
			{
				keyboard.setKey(aKey, ButtonState::HELD);
				break;
			}
			}
		}
	}

	void onMouseMove(GLFWwindow* aWindow, double x, double y)
	{

		if (glfwGetInputMode(aWindow, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
			return;
		auto& mouse = InputManager::getMouse();
		mouse.xPos = x;
		mouse.yPos = y;
	}

	void onMouseButton(GLFWwindow* aWindow, int aButton, int aAction, int aModifiers)
	{

		VulkanWindow& engineWindow = *(VulkanWindow*)glfwGetWindowUserPointer(aWindow);
		auto& mouse = InputManager::getMouse();


		if (!ImGui::GetIO().WantCaptureMouse)
		{
			switch (aAction)
			{
			case GLFW_PRESS:
			{
				mouse.setMouseButton(aButton, ButtonState::PRESSED);
				MouseButtonPressedEvent event(aButton);
				engineWindow.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				mouse.setMouseButton(aButton, ButtonState::RELEASED);
				MouseButtonReleasedEvent event(aButton);
				engineWindow.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				mouse.setMouseButton(aButton, ButtonState::HELD);
				break;
			}
			}
		}
	}
	void onMouseScroll(GLFWwindow* aWindow, double xOffset, double yOffset)
	{
		auto& mouse = InputManager::getMouse();
		// // TODO - MouseScroll Event
		mouse.scrollPos = yOffset;
	}
}
