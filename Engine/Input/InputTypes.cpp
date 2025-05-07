#include "InputTypes.hpp"

namespace Engine
{
	void ButtonStateMap::clear()
	{
		buttonMap.clear();
	}
	ButtonState ButtonStateMap::getButtonState(const Button aButton) const
	{
		const auto& find = buttonMap.find(aButton);
		if (find == buttonMap.end()) {
			return ButtonState::UNKNOWN;
		}
		return find->second;
	}
	void ButtonStateMap::setButtonState(const Button aButton, ButtonState aButtonState)
	{
		// update button states to held, can't use glfw callback, REPEAT events are used for text input
		// (for joystick)
		const ButtonState lastState = getButtonState(aButton);

		if (aButtonState == ButtonState::PRESSED) {
			if (lastState == ButtonState::HELD ||
				lastState == ButtonState::PRESSED) {
				aButtonState = ButtonState::HELD;
			}
		}

		buttonMap[aButton] = aButtonState;
	}
	void ButtonStateMap::Update()
	{
		for (auto& p : buttonMap) {
			switch (p.second) {
			case ButtonState::PRESSED:
				p.second = ButtonState::HELD;
				break;

			case ButtonState::RELEASED:
				p.second = ButtonState::UP;
				break;
			}
		}
	}
}