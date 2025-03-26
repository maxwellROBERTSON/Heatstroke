#pragma once

#include <GLFW/glfw3.h>

// Callbacks

namespace Engine {
	void onWindowClose(GLFWwindow*);
	void joyStickCallback(int, int);
	void onKeyPress(GLFWwindow*, int, int, int, int);
	void onMouseMove(GLFWwindow*, double, double);
	void onMouseButton(GLFWwindow*, int, int, int);
	void onMouseScroll(GLFWwindow*, double, double);
}