#pragma once

#include <vector>

#include "InputTypes.hpp"

namespace Engine
{
	using ControlID = unsigned int;

	struct ButtonControl
	{
		Button button;
		float analogueValue = 0.0f; // for use when key is down
	};
	struct AxisControl
	{
		unsigned axis;
		AxisType axisType;
	};

	struct KeyboardEntry
	{
		Button button; // i know they should be called keys
		float analogueValue = 0.0f; // for use when key is down
	};

	struct MouseEntry
	{
		Button button;
	};

	//struct MouseControl
	//{
	//	// way to add movement?
	//};

	struct GamepadEntry
	{
		InputType inputType;
		AxisType axisType;
		unsigned value;
		float minAnalogueValue = -1.0f;
		float maxAnalogueValue = 1.0f;
	};

	class ControlMap
	{
	public:
		ControlMap() {};
		~ControlMap() {};

		std::map <ControlID, std::vector<KeyboardEntry>> keyboardControlsById;
		std::map <ControlID, std::vector<MouseEntry>> mouseControlsById;
		std::map <ControlID, std::vector<GamepadEntry>> gamepadControlsById;

		float getAnalogueValue(const ControlID control);
		float getDigitalValue(const ControlID control);
		bool isDown(const ControlID control);

		// mapping
		void mapKeyboardButton(const ControlID controlID, const ButtonControl& keyboardControl);
		void mapGamepadButton(const ControlID controlID, const ButtonControl& gamepadControl);
		void mapGamepadAxis(const ControlID controlID, const AxisControl& gamepadAxis);
		void mapMouseButton(const ControlID controlID, const ButtonControl& mouseControl);
	};
}
//	ControlScheme defaultControls;
	//enum Controls {
	//	MoveForward,
	//	MoveBackward,
	//	MoveRight,
	//	MoveLeft,
	//	LookLeft,
	//	LookRight,
	//	LookUp,
	//	LookDown,
	//	Shoot,
	//	Jump,
	//	Pause
	//};
//	defaultControls.map(
//		Controls::MoveForward,
//		HS_GAMEPAD_AXIS_RIGHT_Y,
//		Negative
//	);
//	defaultControls.map(
//		Controls::MoveForward,
//		HS_KEY_W,
//		);
//	defaultControls.map(
//		Controls::MoveBackward,
//		HS_GAMEPAD_AXIS_RIGHT_Y,
//		Positive
//	);
//	defaultControls.map(
//		Controls::MoveForward,
//		HS_KEY_S
//	);
//	defaultControls.map(
//		Controls::Shoot,
//		HS_MOUSE_BUTTON_LEFT
//	);
//	defaultControls.map(
//		Controls::Shoot,
//		HS_GAMEPAD_AXIS_RIGHT_TRIGGER
//	);
//	defaultControls.map(
//		Controls::LookLeft
//		//?? how to connect mouse?
//	);
//	defaultControls.map(
//		Controls::LookLeft,
//		HS_GAMEPAD_AXIS_LEFT_X,
//		Negative
//	);
//}