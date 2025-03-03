#include "Keyboard.hpp"

namespace Engine {
namespace Keyboard {

	std::map<int, std::pair<bool, int>> keys;

	void setKey(int key, std::pair<bool, int> actionAndModifiers) {
		keys[key] = actionAndModifiers;
	}

	std::map<int, std::pair<bool, int>>& getKeyStates() {
		return keys;
	}

}
}