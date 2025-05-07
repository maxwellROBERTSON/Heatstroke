#pragma once

#include <map>

namespace Engine
{

	enum class ButtonState
	{
		UP,
		PRESSED,
		RELEASED,
		HELD,
		UNKNOWN
	};

	enum class AxisType
	{
		FULL,
		NEGATIVE,
		POSITIVE
	};

	enum class InputType
	{
		AXIS,
		BUTTON,
		KEY
	};

	using Button = int;

	class ButtonStateMap
	{
	public:
		std::map<int, ButtonState> buttonMap;
		void clear();
		ButtonState getButtonState(const Button aButton) const;
		void setButtonState(const Button aButton, ButtonState aButtonState);
		void Update();
	};


	using JoystickButtonStateMap = std::map<int, ButtonState>;
	using JoystickAxisStateMap = std::map<int, float>;

	//float buttonStateToAnalogueValue(const ButtonState state)
	//{
	//	switch (state) {
	//	case ButtonState::UP:
	//	case ButtonState::RELEASED:
	//		return 0.0f;

	//	case ButtonState::HELD:
	//	case ButtonState::PRESSED:
	//		return 1.0f;

	//	case ButtonState::UNKNOWN:
	//	default:
	//		return -1.0f;
	//	}
	//}

	//bool buttonStateToDigitalValue(const ButtonState state)
	//{
	//	switch (state) {

	//	case ButtonState::HELD:
	//	case ButtonState::PRESSED:
	//		return true;

	//	case ButtonState::UP:
	//	case ButtonState::RELEASED:
	//	case ButtonState::UNKNOWN:
	//	default:
	//		return false;
	//	}
	//}

	//const char* buttonStateToString(const ButtonState state)
	//{
	//	switch (state) {
	//	case ButtonState::UP:
	//		return "ButtonState::Up";
	//	case ButtonState::PRESSED:
	//		return "ButtonState::Pressed";
	//	case ButtonState::DOWN:
	//		return "ButtonState::Down";
	//	case ButtonState::RELEASED:
	//		return "ButtonState::Released";
	//	case ButtonState::UNKNOWN:
	//	default:
	//		return "ButtonState::Unknow";
	//	}
	//}

}