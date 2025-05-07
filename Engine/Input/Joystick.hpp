#pragma once

#include "Device.hpp"
#include "InputTypes.hpp"

namespace Engine
{

	class Joystick : public Device
	{
	public:
		Joystick();
		Joystick(int index);
		~Joystick() {}
		float getAxisValue(int aAxis) const;
		ButtonState getButtonState(int aButton) const;
		bool isConnected();
		bool isPressed(int aButton); // basically useless for my controller
		bool isHeld(int aButton);
		bool isDown(int aButton); // best
		void Update();
		const std::string& getDeviceName() const override;


		std::string joyStickName;
		int joyStickIndex;
		std::map<int, float> mAxisStates;
		ButtonStateMap mButtonStates;
	};
}