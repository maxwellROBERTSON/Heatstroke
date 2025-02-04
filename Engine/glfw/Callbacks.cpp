#include "Callbacks.hpp"

#include "Keyboard.hpp"
#include "Mouse.hpp"

namespace Engine {

	void registerCallbacks(GLFWwindow* aWindow) {
		glfwSetKeyCallback(aWindow, &onKeyPress);
		glfwSetMouseButtonCallback(aWindow, &onMouseButton);
		glfwSetCursorPosCallback(aWindow, &onMouseMove);
	}

	void onKeyPress(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int aModifiers) {
		switch (aKey) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(aWindow, true);
			break;
		}
	}

	void onMouseButton(GLFWwindow* aWindow, int aButton, int aAction, int aModifiers) {

	}
	
	void onMouseMove(GLFWwindow* aWindow, double x, double y) {
		Mouse::setMousePosition(x, y);
	}
}