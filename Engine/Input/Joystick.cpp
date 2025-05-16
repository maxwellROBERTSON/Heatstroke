#include "Joystick.hpp"

#include <GLFW/glfw3.h>

namespace Engine
{
	Joystick::Joystick() : Device(DeviceType::Joystick)
	{
	}
	Joystick::Joystick(int index) :
		Device(DeviceType::Joystick), joyStickIndex(index)
	{
		joyStickName = glfwGetGamepadName(index);
	}
	float Joystick::getAxisValue(int aAxis) const
	{
		const auto& axis = mAxisStates.find(aAxis);

		if (axis == mAxisStates.end()) {
			return 0.0f;
		}

		return axis->second;
	}
	ButtonState Joystick::getButtonState(int aButton) const
	{
		return mButtonStates.getButtonState(aButton);
	}
	bool Joystick::isConnected()
	{
		return glfwJoystickPresent(joyStickIndex) == GLFW_TRUE;
	}
	bool Joystick::isPressed(int aButton)
	{
		return getButtonState(aButton) == ButtonState::PRESSED;
	}
	bool Joystick::isHeld(int aButton)
	{
		return mButtonStates.getButtonState(aButton) == ButtonState::HELD;

	}
	bool Joystick::isDown(int aButton)
	{
		return mButtonStates.getButtonState(aButton) == ButtonState::PRESSED || mButtonStates.getButtonState(aButton) == ButtonState::HELD;

	}
	void Joystick::Update()
	{
		if (!glfwJoystickPresent(joyStickIndex)) {
			mButtonStates.clear();
			return;
		}

		//mButtonStates.Update();

		int buttonCount;
		const unsigned char* buttons = glfwGetJoystickButtons(joyStickIndex, &buttonCount);

		if (buttons == NULL || buttonCount == 0) {
			return;
		}

		for (int b = 0; b < buttonCount; ++b) {
			const int glfwButtonState = buttons[b];

			ButtonState buttonState;
			switch (glfwButtonState) {
			case GLFW_PRESS:
				buttonState = ButtonState::PRESSED;
				break;

			case GLFW_RELEASE:
				buttonState = ButtonState::RELEASED;
				break;
			}

			mButtonStates.setButtonState(b, buttonState);
		}

		//  Update each axis
		int axisCount;
		const float* axes = glfwGetJoystickAxes(joyStickIndex, &axisCount);
		for (int a = 0; a < axisCount; ++a) {
			mAxisStates[a] = axes[a];
		}
	}

	const std::string& Joystick::getDeviceName() const
	{
		return joyStickName;
	}
}
