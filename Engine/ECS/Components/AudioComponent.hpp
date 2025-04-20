#pragma once

#include <string>
#include <iostream>

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
		ComponentTypes static StaticType() { return ComponentTypes::AUDIO; }
		
		// Get component data
		void GetDataArray(uint8_t*) override;
		
		
		// Setters	
		
		// Set component data
		void SetDataArray(uint8_t*) override;


		//creates a sound device
		SoundDevice* soundDevice;
		//what plays the sound
		SoundSource* speaker;
		//what reads the sound file data
		SoundBuffer* soundBuffer;
		//std::vector<uint32_t> soundClip;
		uint32_t soundClip;

		void AddClip(std::string path);
		//perform this on a thread?
		void playSound();
	};
}

