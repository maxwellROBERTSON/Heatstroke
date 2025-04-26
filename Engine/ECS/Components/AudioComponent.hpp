#pragma once

#include <string>
#include <iostream>
#include "../third_party/AL/al.h"

#include "Component.hpp"

//forward declaration
class SoundDevice;
class SoundSource;
class SoundBuffer;


namespace Engine
{
	class AudioComponent : public Component<AudioComponent>
	{
	public:
		AudioComponent();
		
		void operator=(const AudioComponent& other) override
		{
			std::cout << "assignment operator here" << std::endl;
		}

		// Getters
		
		// Static type getter from Component parent
		ComponentTypes static StaticType() { return ComponentTypes::TYPE_COUNT; }//ComponentTypes::AUDIO; }

		// Static size getter from Component parent
		size_t static StaticSize() { return sizeof(soundClip); }
		
		// Get component data
		void GetDataArray(uint8_t*) override;
		
		// Setters	
		
		// Set component data
		void SetDataArray(uint8_t*) override;

		//current state
		ALint state;

		//creates a sound device
		SoundDevice* soundDevice;
		//what plays the sound
		SoundSource* speaker;
		//what reads the sound file data
		SoundBuffer* soundBuffer;
		
		uint32_t soundClipCurrentlyPlaying;

		//list of audio clips

		//name of clip (key) and buffer (value)
		std::map <std::string, uint32_t> soundClips;

		void addClip(std::string key, std::string path);

		void playSound(std::string key, bool interupt = false);

		bool isPlaying();

		void setActiveClip(std::string key);

		ALuint getState();

		void updateState();
	};
}

