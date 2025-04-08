#pragma once

#include <cassert>
#include <iostream>
#include <string>

#include "../Events/Event.hpp"
#include "../Events/KeyEvent.hpp"
#include "../Events/MouseEvent.hpp"
#include "../Events/WindowEvent.hpp"
#include "../vulkan/VulkanWindow.hpp"
#include "Callbacks.hpp"
#include "InputCodes.hpp"
#include "Joystick.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

namespace Engine
{
	class InputManager
	{
	public:
	protected:
		InputManager() = default;
	public:
		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;
		static Joystick& getJoystick(int index);
		static Keyboard& getKeyboard();
		static Mouse& getMouse();
		static void Update();
		static void RegisterCallbacks(VulkanWindow* window);
		static std::map<int, Joystick> mJoysticks;
		static Keyboard mKeyboard;
		static Mouse mMouse;

	private:
		static InputManager* sInputManager;
	};
}

