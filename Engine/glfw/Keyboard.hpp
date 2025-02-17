#pragma once

#include <map>

namespace Engine {
namespace Keyboard {

	void setKey(int key, std::pair<bool, int> actionAndModifiers);

	std::map<int, std::pair<bool, int>>& getKeyStates();

}
}