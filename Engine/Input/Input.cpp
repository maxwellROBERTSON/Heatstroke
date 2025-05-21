#include "Input.hpp"

#include <GLFW/glfw3.h>

namespace Engine
{

	InputManager* InputManager::sInputManager = new InputManager();
	std::map<int, Joystick> InputManager::mJoysticks;
	Keyboard InputManager::mKeyboard;
	Mouse InputManager::mMouse;
	ControlMap InputManager::DefaultControls;
	InputDevice InputManager::mInputDevice = InputDevice::KBM;
	float InputManager::ControllerDeadzone = 0.05f;

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

	void InputManager::setInputDevivce(InputDevice inputDevice)
	{
		mInputDevice = inputDevice;
	}

	InputDevice& InputManager::getInputDevice()
	{
		return mInputDevice;
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
		DefaultControls.mapKeyboardButton(Controls::MoveLeft, { HS_KEY_A, -1.0f });
		DefaultControls.mapKeyboardButton(Controls::MoveRight, { HS_KEY_D, 1.0f });
		DefaultControls.mapKeyboardButton(Controls::Jump, { HS_KEY_SPACE, 1.0f });
		DefaultControls.mapKeyboardButton(Controls::SwapWeapon, { HS_KEY_X, 1.0f });
		DefaultControls.mapKeyboardButton(Controls::Sprint, { HS_KEY_LEFT_SHIFT, 1.0f });
		DefaultControls.mapMouseButton(Controls::Shoot, { HS_MOUSE_BUTTON_LEFT });

		//if (InputManager::hasJoysticksConnected())
		//{
		//	DefaultControls.mapGamepadButton(Controls::Reload, { HS_GAMEPAD_BUTTON_Y, 1.0f });
		//	DefaultControls.mapGamepadButton(Controls::Jump, { HS_GAMEPAD_BUTTON_A, 1.0f });
		//	DefaultControls.mapGamepadAxis(Controls::Shoot, { HS_GAMEPAD_AXIS_RIGHT_TRIGGER, AxisType::POSITIVE });
		//	DefaultControls.mapGamepadAxis(Controls::MoveForward, { HS_GAMEPAD_AXIS_RIGHT_Y,AxisType::NEGATIVE });
		//	DefaultControls.mapGamepadAxis(Controls::MoveBackward, { HS_GAMEPAD_AXIS_RIGHT_Y,AxisType::POSITIVE });
		//}
	}

	bool InputManager::Action(const ControlID action)
	{
		return getDefaultControls().isDown(action);
	}

	void InputManager::Update()
	{

		mKeyboard.getKeyboardState().Update();
		mMouse.getMouseState().Update();

		for (int j = 0; j < GLFW_JOYSTICK_LAST; ++j) {
			if (glfwJoystickPresent(j) == GLFW_TRUE) {
				auto& joystick = static_cast<Joystick&>(getJoystick(j));
				joystick.Update();
			}
		}

		glfwPollEvents();

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
	bool InputManager::IsDown(int aKey)
	{
		return mKeyboard.isDown(aKey);
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