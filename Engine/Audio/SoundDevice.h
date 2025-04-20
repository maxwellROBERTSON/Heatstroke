#pragma once

//dependencies
#include "../third_party/AL/alc.h"

//Initialise audio
//Uses singleton pattern
class SoundDevice
{
public:
	static SoundDevice* get();

private:
	SoundDevice();
	~SoundDevice();

	ALCdevice* p_ALCDevice;
	ALCcontext* p_ALCcontext;
};

