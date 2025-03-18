#pragma once

#include <GLFW/glfw3.h>

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "../Core/Game.h"

namespace Engine {

	class Game;

	void registerCallbacks(GLFWwindow* aWindow, Engine::Game* game);

	void onKeyPress(GLFWwindow*, int, int, int, int);
	void onMouseButton(GLFWwindow*, int, int, int);
	void onMouseMove(GLFWwindow*, double, double);
	void joyStickCallback(int, int);
}