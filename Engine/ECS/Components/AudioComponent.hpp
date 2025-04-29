#pragma once

#include <string>
#include <iostream>

#include "../third_party/AL/al.h"
#include "../EntityManager.hpp"
#include "Component.hpp"

#include "../EntityManager.hpp"

namespace Engine
{
	class EntityManager;
	class Entity;
	class SoundDevice;
	class SoundSource;
	class SoundBuffer;

}
namespace Engine
{
	class AudioComponent : public Component<AudioComponent>
	{
	public:
		AudioComponent();
		AudioComponent(EntityManager* entityManager, Entity* entity);
		
		void operator=(const AudioComponent& other) override
		{
			std::cout << "assignment operator here" << std::endl;
		}

		// Getters
		
		// Static type getter from Component parent
		ComponentTypes static StaticType() { return ComponentTypes::AUDIO; }

		// Static size getter from Component parent
		size_t static StaticSize() { return sizeof(soundClipCurrentlyPlaying); }
		
		// Get component data
		void GetDataArray(uint8_t*) override;
		
		// Setters
		
		// Set component data
		void SetDataArray(uint8_t*) override;

		// Set component has changed in entity manager
		void SetComponentHasChanged();

		// Toggle has changed boolean
		void ToggleHasChanged() { hasChanged = !hasChanged; }

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

	private:
		// EntityManager pointer
		EntityManager* entityManager;
		// Entity pointer
		Entity* entity;

		// If component has changed since last network update
		bool hasChanged = false;
	};
}
