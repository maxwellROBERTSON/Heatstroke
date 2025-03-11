#include "Input.hpp"

namespace Engine
{

	InputManager* InputManager::sInputManager = new InputManager();
	std::map<int, Joystick> InputManager::mJoysticks;
	Keyboard InputManager::mKeyboard;
	Mouse InputManager::mMouse;


	Joystick& InputManager::getJoystick(int index)
	{
		if (mJoysticks.find(index) == mJoysticks.end()) {
			mJoysticks.try_emplace(index, index);
		}

		return mJoysticks.at(index);
	}

	Keyboard& InputManager::getKeyboard()
	{
		return mKeyboard;
	}

	Mouse& InputManager::getMouse()
	{
		return mMouse;
	}

	void InputManager::Update()
	{
		glfwPollEvents();

		for (int j = 0; j < GLFW_JOYSTICK_LAST; ++j) {
			if (glfwJoystickPresent(j) == GLFW_TRUE) {
				auto& joystick = static_cast<Joystick&>(getJoystick(j));
				joystick.Update();
			}
		}
	}
	void InputManager::RegisterCallbacks(VulkanWindow* window)
	{
		glfwSetWindowCloseCallback(window->window, [](GLFWwindow* aWindow)
			{
				VulkanWindow& engineWindow = *(VulkanWindow*)glfwGetWindowUserPointer(aWindow);
				Engine::WindowCloseEvent event;
				engineWindow.EventCallback(event);
			});

		glfwSetJoystickCallback(&joyStickCallback);
		glfwSetKeyCallback(window->window, &onKeyPress);
		glfwSetCursorPosCallback(window->window, &onMouseMove);
		glfwSetMouseButtonCallback(window->window, &onMouseButton);

	}
}