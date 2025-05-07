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
#include "Controls.hpp"
#include "InputCodes.hpp"
#include "Joystick.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

namespace Engine
{
	//enum ActionType
	//{
	//	Accept,
	//	Decline
	//};

	enum Controls {
		MoveForward,
		MoveBackward,
		MoveRight,
		MoveLeft,
		LookLeft,
		LookRight,
		LookUp,
		LookDown,
		Shoot,
		Reload,
		Jump,
		Pause
	};

	class InputManager
	{
	public:
	protected:
		InputManager() = default;
	public:
		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;

		// Getters
		static Joystick& getJoystick(int index);
		static Keyboard& getKeyboard();
		static void addJoysitck(int index);
		static void removeJoystick(int index);
		static Mouse& getMouse();
		static bool hasJoysticksConnected();

		static ControlMap& getDefaultControls();

		static void InitDefaultControls();
		//static std::map<std::string, std::pair<int, int>> mActionMap;
		//static std::map<ActionType, std::pair<int, int>> mActionMap;

		// Setters
		//static void addAction(const std::string& actionName, int aKey, int aButton);
		//static void bindAction(const ActionType& action, int aKey, int aButton);

		// Input
		static bool IsPressed(int aKey);
		//static bool Action(const std::string& actionName);
		static bool Action(const ControlID action);
		// ?
		static void Update();
		static void RegisterCallbacks(VulkanWindow* window);


		//private:
		static InputManager* sInputManager;
		static Keyboard mKeyboard;
		static std::map<int, Joystick> mJoysticks;
		static Mouse mMouse;
		static ControlMap DefaultControls;


	};
}

