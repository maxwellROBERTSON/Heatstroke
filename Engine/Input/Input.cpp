#include "Input.hpp"

#include <GLFW/glfw3.h>

namespace Engine
{

	InputManager* InputManager::sInputManager = new InputManager();
	std::map<int, Joystick> InputManager::mJoysticks;
	Keyboard InputManager::mKeyboard;
	Mouse InputManager::mMouse;
	ControlMap InputManager::DefaultControls;
	//std::map<std::string, std::pair<int, int>> InputManager::mActionMap;
	// 
	//std::map<ActionType, std::pair<int, int>> InputManager::mActionMap;


	Joystick& InputManager::getJoystick(int index)
	{
		if (mJoysticks.find(index) == mJoysticks.end()) {
			mJoysticks.try_emplace(index, index);
		}

		return mJoysticks.at(index);
	}

	Keyboard& InputManager::getKeyboard()
	{
		return mKeyboard;
	}

	void InputManager::addJoysitck(int index)
	{
		mJoysticks[index] = Joystick(index);

	}

	void InputManager::removeJoystick(int index)
	{
		mJoysticks.erase(index);
	}

	Mouse& InputManager::getMouse()
	{
		return mMouse;
	}

	bool InputManager::hasJoysticksConnected()
	{
		//return !mJoysticks.empty();
		return (mJoysticks.size() != 0);
	}

	ControlMap& InputManager::getDefaultControls()
	{
		return DefaultControls;
	}

	void InputManager::InitDefaultControls()
	{
		DefaultControls.mapKeyboardButton(Controls::MoveForward, { HS_KEY_W, 1.0f });
		DefaultControls.mapKeyboardButton(Controls::MoveBackward, { HS_KEY_S, -1.0f });
		DefaultControls.mapKeyboardButton(Controls::Reload, { HS_KEY_R, 1.0f });
		DefaultControls.mapGamepadButton(Controls::Reload, { HS_GAMEPAD_BUTTON_Y, 1.0f });
		//DefaultControls.mapGamepadAxis(Controls::MoveForward, { HS_GAMEPAD_AXIS_RIGHT_Y,AxisType::NEGATIVE, 0.0f, 0.49f });
		//DefaultControls.mapGamepadAxis(Controls::MoveBackward, { HS_GAMEPAD_AXIS_RIGHT_Y,AxisType::POSITIVE, 0.5f, 1.0f });
		DefaultControls.mapKeyboardButton(Controls::MoveLeft, { HS_KEY_A, -1.0f });
		DefaultControls.mapKeyboardButton(Controls::MoveRight, { HS_KEY_D, 1.0f });
		DefaultControls.mapKeyboardButton(Controls::Jump, { HS_KEY_SPACE, 1.0f });
		DefaultControls.mapGamepadButton(Controls::Jump, { HS_GAMEPAD_BUTTON_A, 1.0f });
		DefaultControls.mapMouseButton(Controls::Shoot, { HS_MOUSE_BUTTON_LEFT });
		DefaultControls.mapGamepadAxis(Controls::Shoot, { HS_GAMEPAD_AXIS_RIGHT_TRIGGER, AxisType::FULL, 0.0f, 1.0f });
	}

	bool InputManager::Action(const ControlID action)
	{
		return getDefaultControls().isDown(action);
	}

	void InputManager::Update()
	{
		glfwPollEvents();

		//mKeyboard.getKeyboardState().Update();
		mMouse.getMouseState().Update();

		for (int j = 0; j < GLFW_JOYSTICK_LAST; ++j) {
			if (glfwJoystickPresent(j) == GLFW_TRUE) {
				auto& joystick = static_cast<Joystick&>(getJoystick(j));
				joystick.Update();
			}
		}
	}
	void InputManager::RegisterCallbacks(VulkanWindow* window)
	{
		glfwSetWindowCloseCallback(window->window, &onWindowClose);
		glfwSetJoystickCallback(&joyStickCallback);
		glfwSetKeyCallback(window->window, &onKeyPress);
		glfwSetCursorPosCallback(window->window, &onMouseMove);
		glfwSetMouseButtonCallback(window->window, &onMouseButton);
		glfwSetScrollCallback(window->window, &onMouseScroll);
	}
	//void InputManager::addAction(const std::string& actionName, int aKey, int aButton)
	//{
	//	mActionMap[actionName] = std::make_pair(aKey, aButton);
	//}
	//void InputManager::bindAction(const ActionType& action, int aKey, int aButton)
	//{
	//	mActionMap[action] = std::make_pair(aKey, aButton);
	//}
	bool InputManager::IsPressed(int aKey)
	{
		return mKeyboard.isPressed(aKey);
	}
	//bool InputManager::Action(const ActionType& action)
	//{
	//	std::pair<int, int> inputs = mActionMap[action];
	//	return mKeyboard.isPressed(inputs.first) || mJoysticks[0].isPressed(inputs.second);
	//}
	//bool InputManager::Action(const std::string& actionName)
	//{
	//	std::pair<int, int> inputs = mActionMap[actionName];
	//	return mKeyboard.isPressed(inputs.first) || mJoysticks[0].isPressed(inputs.second);
	//}
}