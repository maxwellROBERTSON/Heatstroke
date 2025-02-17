#include "Callbacks.hpp"

#include "Keyboard.hpp"
#include "Mouse.hpp"

#include <iostream>

namespace Engine {

	void registerCallbacks(GLFWwindow* aWindow) {
		glfwSetKeyCallback(aWindow, &onKeyPress);
		glfwSetMouseButtonCallback(aWindow, &onMouseButton);
		glfwSetCursorPosCallback(aWindow, &onMouseMove);
		glfwSetJoystickCallback(&joyStickCallback);
	}

	void onKeyPress(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int aModifiers) {
		if (aAction == GLFW_PRESS)
		{
			switch (aKey) {
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(aWindow, true);
				break;
			case GLFW_KEY_SPACE:
				std::cout << "SPACE" << std::endl;
				break;
			default:
				std::cout << glfwGetKeyName(aKey, aScanCode) << std::endl;
			}
		}
	}

	void onMouseButton(GLFWwindow* aWindow, int aButton, int aAction, int aModifiers) {

	}

	void onMouseMove(GLFWwindow* aWindow, double x, double y) {
		Mouse::setMousePosition(x, y);
	}

	void joyStickCallback(int jid, int event)
	{
		if (event == GLFW_CONNECTED)
		{
			std::cout << glfwGetJoystickName(jid) << " has been connected" << std::endl;
		}
		else
		{
			std::cout << " Controller has been disconnected" << std::endl;
			// Breaks - assuming that once it disconnects get name doesn't work
			//std::cout << glfwGetJoystickName(jid) << "has been disconnected" << std::endl;
		}
	}
}