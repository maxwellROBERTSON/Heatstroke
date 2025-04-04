#pragma once

#include "../Input/Device.hpp"

namespace Engine {

	class Joystick : public Device
	{
	public:
		Joystick(int index);
		//Joystick(DeviceType aDeviceType, const std::string& name, int index);
		~Joystick() {}
		float getAxisValue(int aAxis) const;
		ButtonState getButtonState(int aButton) const;
		bool isConnected();
		bool isPressed(int aButton);
		void Update();
		const std::string& getDeviceName() const override;


		std::string joyStickName;
		int joyStickIndex;
		std::map<int, float> mAxisStates;
		std::map<int, ButtonState> mButtonStates;
	};
}