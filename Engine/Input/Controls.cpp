#include "Controls.hpp"
#include "Input.hpp"

namespace Engine
{

	static float getAxisValue(
		const float value,
		const AxisType axisType,
		const float minAnalogueValue,
		const float maxAnalogueValue
	) {
		float v;

		switch (axisType) {
		case AxisType::FULL:
			v = value;
			break;
		case AxisType::POSITIVE:
			v = value > 0.0f ? value : 0.0f;
			break;
		case AxisType::NEGATIVE:
			v = value < 0.0f ? value : 0.0f;
			break;
		default:
			break;
		}

		const float inStart = -1.0f;
		const float inEnd = 1.0f;

		return
			(v - inStart) / (inEnd - inStart)
			* (maxAnalogueValue - minAnalogueValue)
			+ minAnalogueValue;
	}
	float ControlMap::getAnalogueValue(const ControlID control)
	{
		return 0.0f;
	}
	float ControlMap::getDigitalValue(const ControlID control)
	{
		return 0.0f;
	}
	bool ControlMap::isDown(const ControlID control)
	{
		bool keyboardIsDown = false;
		bool mouseIsDown = false;
		bool controllerIsDown = false;

		// keboard check
		const auto& findKeyboard = keyboardControlsById.find(control);

		if (!(findKeyboard == keyboardControlsById.end())) {
			const auto& entries = findKeyboard->second;

			for (const auto& entry : entries) {
				if (InputManager::getKeyboard().isDown(entry.button)) {
					keyboardIsDown = true;
				}
			}
		}
		// mouse check
		const auto& findMouse = mouseControlsById.find(control);

		if (!(findMouse == mouseControlsById.end())) {
			const auto& entries = findMouse->second;

			for (const auto& entry : entries) {
				if (InputManager::getMouse().isDown(entry.button)) {
					mouseIsDown = true;
				}
			}
		}

		//controller check
		if (InputManager::hasJoysticksConnected())
		{
			const auto& findGamepad = gamepadControlsById.find(control);

			if (!(findGamepad == gamepadControlsById.end())) {
				const auto& entries = findGamepad->second;

				float deadZone = 0.03f;
				for (const auto& entry : entries) {
					switch (entry.inputType)
					{
					case InputType::AXIS:
					{
						//if (entry.axisType == AxisType::FULL)
						//{
						//	std::cout << (entry.value >= -deadZone && entry.value <= deadZone) ? "True" ? "False";
						//	controllerIsDown = entry.value == 0.0f;
						//}
						if (entry.axisType == AxisType::NEGATIVE)
						{
							std::cout << (entry.value <= -deadZone ? "True" : "False") << std::endl;
							controllerIsDown = entry.value < 0.0f;
						}
						else if (entry.axisType == AxisType::POSITIVE)
						{
							std::cout << (entry.value >= deadZone ? "True" : "False") << std::endl;
							controllerIsDown = entry.value < 0.0f;
						}
						//else
							//std::cout << "UNEXPECTED" << std::endl;
					}
					break;
					case InputType::BUTTON:
					{
						if (InputManager::getJoystick(0).isDown(entry.value)) {
							controllerIsDown = true;
						}
					}
					break;
					}
				}
			}
		}
		return keyboardIsDown || mouseIsDown || controllerIsDown;
	}

	void ControlMap::mapKeyboardButton(const ControlID controlID, const ButtonControl& keyboardControl)
	{
		KeyboardEntry entry{};
		entry.button = keyboardControl.button;
		entry.analogueValue = keyboardControl.analogueValue;
		keyboardControlsById[controlID].push_back(entry);
	}

	void ControlMap::mapGamepadButton(const ControlID controlID, const ButtonControl& gamepadControl)
	{
		GamepadEntry entry{};
		entry.inputType = InputType::BUTTON;
		entry.value = gamepadControl.button;
		gamepadControlsById[controlID].push_back(entry);
	}

	void ControlMap::mapGamepadAxis(const ControlID controlID, const AxisControl& gamepadAxis)
	{
		GamepadEntry entry{};
		entry.inputType = InputType::AXIS;
		entry.value = gamepadAxis.axis;
		gamepadControlsById[controlID].push_back(entry);
	}

	void ControlMap::mapMouseButton(const ControlID controlID, const ButtonControl& mouseControl)
	{
		MouseEntry entry{};
		entry.button = mouseControl.button;
		mouseControlsById[controlID].push_back(entry);
	}

}