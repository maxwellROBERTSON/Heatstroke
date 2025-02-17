#pragma once

#include <GLFW/glfw3.h>

namespace Engine {

	void registerCallbacks(GLFWwindow* aWindow);

	void onKeyPress(GLFWwindow*, int, int, int, int);
	void onMouseButton(GLFWwindow*, int, int, int);
	void onMouseMove(GLFWwindow*, double, double);
	void joyStickCallback(int, int);
}