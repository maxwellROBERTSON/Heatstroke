#pragma once

#include "Device.hpp"
#include "InputTypes.hpp"

namespace Engine {

	class Mouse : public Device
	{
	public:
		Mouse() : Device(DeviceType::Mouse) {}
		~Mouse() {}

		const std::string& getDeviceName() const override;

		//ButtonState getButtonState(int aButton);
		ButtonStateMap& getMouseState();
		float getXPos();
		float getYPos();
		float getScrollPos();
		void setMouseButton(Button aButton, ButtonState aButtonState);


		bool isPressed(int aButton);
		bool isHeld(int aButton);
		bool isDown(int aButton);

		float scrollPos;
		float xPos;
		float yPos;


		ButtonStateMap mButtonStates;


		//void setXPos(double x) { xPos = x; }
//void setYPos(double y) { yPos = y; }
//void setScroll(double offset) { scrollPos = offset; }
	};
}
