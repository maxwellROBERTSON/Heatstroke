#include "Keyboard.hpp"

namespace Engine {
	const std::string& Keyboard::getDeviceName() const
	{
		return "Keyboard";
	}

	ButtonStateMap& Keyboard::getKeyboardState()
	{
		return mKeyStates;
	}

	bool Keyboard::isPressed(Button aButton)
	{
		return mKeyStates.getButtonState(aButton) == ButtonState::PRESSED;
	}

	bool Keyboard::isHeld(Button aButton)
	{
		return mKeyStates.getButtonState(aButton) == ButtonState::HELD;
	}

	bool Keyboard::isDown(Button aButton)
	{
		return mKeyStates.getButtonState(aButton) == ButtonState::PRESSED || mKeyStates.getButtonState(aButton) == ButtonState::HELD;
	}

	void Keyboard::setKey(Button aButton, ButtonState aButtonState)
	{
		mKeyStates.setButtonState(aButton, aButtonState);
	}
}