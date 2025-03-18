#pragma once

#include "../Input/Device.hpp"
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

		std::map<int, ButtonState>& getKeyboardState();
		bool isPressed(int);
		void setKey(int, ButtonState);
		std::map<int, ButtonState> mKeyStates;

	};
}
