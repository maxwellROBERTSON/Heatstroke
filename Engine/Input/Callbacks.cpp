#include "Callbacks.hpp"

#include "Keyboard.hpp"
#include "Mouse.hpp"

#include <iostream>

#include "../Events/Event.h"
#include "../Events/KeyEvent.h"
#include "../Events/MouseEvent.h"
#include "../Events/WindowEvent.h"

namespace Engine {

	void registerCallbacks(GLFWwindow* aWindow) {
		glfwSetKeyCallback(aWindow, &onKeyPress);
		glfwSetMouseButtonCallback(aWindow, &onMouseButton);
		glfwSetCursorPosCallback(aWindow, &onMouseMove);
		glfwSetJoystickCallback(&joyStickCallback);
	}
	void onKeyPress(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int aModifiers) {
		switch (aKey) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(aWindow, true);
			break;
		}

		if (aAction == GLFW_PRESS)
			Keyboard::setKey(aKey, std::pair(true, aModifiers));
		else if (aAction == GLFW_RELEASE)
			Keyboard::setKey(aKey, std::pair(false, aModifiers));
	}

	void onMouseButton(GLFWwindow* aWindow, int aButton, int aAction, int aModifiers) {
		switch (aButton) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			if (aAction == GLFW_PRESS) {
				if (glfwGetInputMode(aWindow, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else if (glfwGetInputMode(aWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
			}
			break;
		}
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

// #include "Callbacks.hpp"

// #include "Keyboard.hpp"
// #include "Mouse.hpp"

// namespace Engine {

// 	void registerCallbacks(GLFWwindow* aWindow) {
// 		glfwSetKeyCallback(aWindow, &onKeyPress);
// 		glfwSetMouseButtonCallback(aWindow, &onMouseButton);
// 		glfwSetCursorPosCallback(aWindow, &onMouseMove);
// 	}

// 	void onKeyPress(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int aModifiers) {
// 		switch (aKey) {
// 		case GLFW_KEY_ESCAPE:
// 			glfwSetWindowShouldClose(aWindow, true);
// 			break;
// 		}

// 		if (aAction == GLFW_PRESS)
// 			Keyboard::setKey(aKey, std::pair(true, aModifiers));
// 		else if (aAction == GLFW_RELEASE)
// 			Keyboard::setKey(aKey, std::pair(false, aModifiers));
// 	}

// 	void onMouseButton(GLFWwindow* aWindow, int aButton, int aAction, int aModifiers) {
// 		switch (aButton) {
// 		case GLFW_MOUSE_BUTTON_RIGHT:
// 			if (aAction == GLFW_PRESS) {
// 				if (glfwGetInputMode(aWindow, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
// 					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
// 				}
// 				else if (glfwGetInputMode(aWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
// 					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
// 				}
// 			}
// 			break;
// 		}
// 	}

// 	void onMouseMove(GLFWwindow* aWindow, double x, double y) {
// 		Mouse::setMousePosition(x, y);
// 	}