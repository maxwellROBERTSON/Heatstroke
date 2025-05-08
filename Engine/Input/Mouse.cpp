#include "Mouse.hpp"

namespace Engine {
	const std::string& Mouse::getDeviceName() const
	{
		return "Mouse";

	}
	//ButtonState Mouse::getButtonState(int aButton)
	//{
	//	return mButtonStates[aButton];
	//}
	ButtonStateMap& Mouse::getMouseState()
	{
		return mButtonStates;
	}
	float Mouse::getXPos()
	{
		return xPos;
	}
	float Mouse::getYPos()
	{
		return yPos;
	}
	float Mouse::getScrollPos()
	{
		return scrollPos;
	}
	void Mouse::setMouseButton(Button aButton, ButtonState aButtonState)
	{
		mButtonStates.setButtonState(aButton, aButtonState);
	}
	bool Mouse::isPressed(int aButton)
	{
		return mButtonStates.getButtonState(aButton) == ButtonState::PRESSED;
	}
	bool Mouse::isHeld(int aButton)
	{
		return mButtonStates.getButtonState(aButton) == ButtonState::HELD;

	}
	bool Mouse::isDown(int aButton)
	{
		return mButtonStates.getButtonState(aButton) == ButtonState::PRESSED || mButtonStates.getButtonState(aButton) == ButtonState::HELD;

	}
}

//namespace Mouse {
//
//	double mouseX;
//	double mouseY;
//
//	void setMousePosition(double x, double y) {
//		mouseX = x;
//		mouseY = y;
//	}
//
//	float getX() {
//		return float(mouseX);
//	}
//
//	float getY() {
//		return float(mouseY);
//	}
//
//}