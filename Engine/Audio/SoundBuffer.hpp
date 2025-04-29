#pragma once

//dependencies
#include "al.h"
#include <vector>

namespace Engine
{
	//storage of sound file
	//Uses singleton pattern. Reads file 
	class SoundBuffer
	{
	public:
		static SoundBuffer* get();

		ALuint addSoundEffect(const char* filename);
		bool removeSoundEffect(const ALuint& buffer);

	private:
		SoundBuffer();
		~SoundBuffer();


		std::vector<ALuint> p_SoundEffectBuffers;
	};
}
