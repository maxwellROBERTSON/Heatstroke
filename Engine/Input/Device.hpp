#pragma once

#include <iostream>
#include <map>
#include <string>



namespace Engine {

	enum class DeviceType
	{
		Joystick,
		Keyboard,
		Mouse
	};

	class Device
	{
	public:
		DeviceType mDeviceType;
		Device(const DeviceType& aDeviceType)
			: mDeviceType(aDeviceType) {
		}

		virtual ~Device() {}

		DeviceType getDeviceType() const {
			return mDeviceType;
		}

		virtual const std::string& getDeviceName() const = 0;
	};
}