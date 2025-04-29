#pragma once

//dependencies
#include "alc.h"

namespace Engine
{
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
}
