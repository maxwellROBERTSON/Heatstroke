#include "SoundDevice.hpp"
#include "../third_party/AL/al.h"
#include "../third_party/AL/alc.h"

#include <stdio.h>

//get singleton instance
SoundDevice* SoundDevice::get()
{
	static SoundDevice* snd_device = new SoundDevice();
	return snd_device;
}

SoundDevice::SoundDevice()
{
	//opens a device by name and returns a pointer to a ALCdevice object
	p_ALCDevice = alcOpenDevice(nullptr); //nullptr = get default device

	//check if the device was opened sucessfully
	if (!p_ALCDevice)
	{
		throw("Failed to get sound device");
	}

	//create a context with the device we just created (p_ALCDevice)
	//optional pass in a pointer to a set of attributes, not used here
	p_ALCcontext = alcCreateContext(p_ALCDevice, nullptr); //set context
	if (!p_ALCcontext)
	{
		throw("Failed to set sound context");
	}

	//if successful makes the context we created(p_ALCcontext) current
	if (!alcMakeContextCurrent(p_ALCcontext))
	{
		throw("Failed to make context current");
	}

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(p_ALCDevice, "ALC_EMUMERATE_ALL_EXT"))
		name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
		name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);

	printf("Opened \"s\"\n", name);
}

SoundDevice::~SoundDevice()
{
	if (alcMakeContextCurrent(nullptr))
	{
		throw("Failed to set context to nullptr");
	}

	alcDestroyContext(p_ALCcontext);

	if (p_ALCcontext)
	{
		throw("Failed to unset during close");
	}

	if (!alcCloseDevice(p_ALCDevice))
	{
		throw("Failed to close sound device");
	}
}