#pragma once

#include <GLFW/glfw3.h>

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "../Core/Game.hpp"

namespace Engine {
	class Game;

	void onWindowClose(GLFWwindow*);
	void joyStickCallback(int, int);
	void onKeyPress(GLFWwindow*, int, int, int, int);
	void onMouseMove(GLFWwindow*, double, double);
	void onMouseButton(GLFWwindow*, int, int, int);
	void onMouseScroll(GLFWwindow*, double, double);
}