#pragma once

#include "Device.hpp"
#include "InputTypes.hpp"
#include <iostream>
#include <map>
#include <string>

namespace Engine
{

	class Keyboard : public Device
	{
	public:
		Keyboard() : Device(DeviceType::Keyboard) {}
		~Keyboard() {}

		const std::string& getDeviceName() const override;

		ButtonStateMap& getKeyboardState();
		bool isPressed(Button aButton); // inconsistent
		bool isHeld(Button aButton); // inconsistent
		bool isDown(Button aButton); // best
		ButtonStateMap mKeyStates;
		void setKey(Button aButton, ButtonState aButtonState);

	};
}
