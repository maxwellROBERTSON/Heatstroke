#include "Mouse.hpp"

namespace Engine {
	const std::string& Mouse::getDeviceName() const
	{
		return "Mouse";

	}
	ButtonState Mouse::getButtonState(int aButton)
	{
		return mButtonStates[aButton];
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
	bool Mouse::isPressed(int aButton)
	{
		return getButtonState(aButton) == ButtonState::PRESSED;
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