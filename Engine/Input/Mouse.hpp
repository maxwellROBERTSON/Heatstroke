#pragma once

#include "Device.hpp"

namespace Engine {

	class Mouse : public Device
	{
	public:
		Mouse() : Device(DeviceType::Mouse) {}
		~Mouse() {}

		const std::string& getDeviceName() const override;

		ButtonState getButtonState(int aButton);
		float getXPos();
		float getYPos();
		float getScrollPos();
		bool isPressed(int aButton);

		float scrollPos;
		float xPos;
		float yPos;

		std::map<int, ButtonState> mButtonStates;

	};
}

//namespace Mouse {
//
//	void setMousePosition(double x, double y);
//
//	float getX();
//	float getY();
//}