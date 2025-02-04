#include "Mouse.hpp"

namespace Engine {
namespace Mouse {

	double mouseX;
	double mouseY;

	void setMousePosition(double x, double y) {
		mouseX = x;
		mouseY = y;
	}

}
}