#include "Keyboard.hpp"

namespace Engine {
	const std::string& Keyboard::getDeviceName() const
	{
		return "Keyboard";
	}

	std::map<int, ButtonState>& Keyboard::getKeyboardState()
	{
		return mKeyStates;
	}

	bool Keyboard::isPressed(int aKey)
	{
		return (mKeyStates[aKey] == ButtonState::PRESSED);
	}

	void Keyboard::setKey(int aKey, ButtonState aButtonState)
	{
		mKeyStates[aKey] = aButtonState;
	}

}