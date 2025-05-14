#include "AudioComponent.hpp"
#include "../Audio/SoundDevice.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "../Audio/SoundSource.hpp"

namespace Engine
{
	// Constructors

	AudioComponent::AudioComponent() : entityManager(nullptr), entity(nullptr)
	{
		soundDevice = SoundDevice::get();
		soundBuffer = SoundBuffer::get();
		speaker = new SoundSource();
		soundClipCurrentlyPlaying = 0;
		state = AL_INITIAL;
	}
	AudioComponent::AudioComponent(Engine::EntityManager* entityManager, Engine::Entity* entity) : entityManager(entityManager), entity(entity)
	{
		soundDevice = SoundDevice::get();
		soundBuffer = SoundBuffer::get();
		speaker = new SoundSource();
		soundClipCurrentlyPlaying = 0;
		state = AL_INITIAL;
	}

	// Get component data
	void AudioComponent::GetDataArray(uint8_t* data)
	{
		uint32_t offset = 0;

		std::memcpy(data + offset, &soundClipCurrentlyPlaying, sizeof(soundClipCurrentlyPlaying));
		offset += sizeof(soundClipCurrentlyPlaying);
	}

	// Set component data
	void AudioComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		if (std::memcmp(&soundClipCurrentlyPlaying, data + offset, sizeof(soundClipCurrentlyPlaying)) != 0)
		{
			std::memcpy(&soundClipCurrentlyPlaying, data + offset, sizeof(soundClipCurrentlyPlaying));
			SetComponentHasChanged();
		}
		offset += sizeof(soundClipCurrentlyPlaying);
	}

	// Set component has changed in entity manager
	void AudioComponent::SetComponentHasChanged()
	{
		if (!hasChanged)
		{
			entityManager->AddChangedComponent(StaticType(), entity);
			hasChanged = true;
		}
	}

	void AudioComponent::addClip(std::string key, std::string path)
	{
		soundClips.insert({ key, soundBuffer->get()->addSoundEffect(path.c_str()) });
	}

	void AudioComponent::playSound(std::string key, bool interupt)
	{
		if (soundClips[key] == NULL)
		{
			// DLOG("No audio clip with that key value exists");
			return;
		}

		ALuint currentState = getState();

		setActiveClip(key);

		speaker->Play(soundClipCurrentlyPlaying);

		//is a clip currently playing?
		//if (isPlaying() == false)
		//{
		//	//no - set new active clip
		//	//setActiveClip(key);

		//	//play new clip
		//	speaker->Play(soundClipCurrentlyPlaying);

		//	updateState();
		//}
		//else if (interupt == true)
		//{
		//	updateState();

		//	//stop current clip
		//	speaker->Stop();

		//	DLOG("Stopped file playing" << std::endl;

		//	//set new active clip
		//	//setActiveClip(key);

		//	//play new clip
		//	speaker->Play(soundClipCurrentlyPlaying);

		//	updateState();
		//}
		//else
		//{
		//	//yes - return
		//	DLOG("Audio alreading playing through this source" << std::endl;
		//	return;
		//}
	}

	bool AudioComponent::isPlaying()
	{
		//update the state
		/*if (soundClipCurrentlyPlaying != 0)
		{
			alGetSourcei(soundClipCurrentlyPlaying, AL_SOURCE_STATE, &state);
			ALenum error = alGetError();
			if (error != AL_NO_ERROR) {
				std::cerr << "OpenAL error: " << error << std::endl;
			}
		}*/

		getState();

		if (ALenum res = alGetError(); res != AL_NO_ERROR)
		{
			DLOG("ERROR from alGetError() " << res);

			if (res == ALC_NO_ERROR) DLOG("ALC_NO_ERROR");
			if (res == ALC_INVALID_DEVICE) DLOG("ALC_INVALID_DEVICE");
			if (res == ALC_INVALID_CONTEXT) DLOG("ALC_INVALID_CONTEXT");
			if (res == ALC_INVALID_ENUM) DLOG("ALC_INVALID_ENUM");
			if (res == ALC_INVALID_VALUE) DLOG("ALC_INVALID_VALUE");
			if (res == ALC_OUT_OF_MEMORY) DLOG("ALC_OUT_OF_MEMORY");
		}

		//if no clip has been played yet default value is AL_INITIAL thus nothing is playing so return false
		if (state == AL_INITIAL) return false;

		if (state == AL_PLAYING)
		{
			//clip currently playing
			return true;
		}

		//no clip playing
		return false;
	}


	void AudioComponent::setActiveClip(std::string key)
	{
		soundClipCurrentlyPlaying = soundClips[key];
	}


	ALuint AudioComponent::getState()
	{
		//see speaker buffers value
		ALuint buffer = speaker->GetBuffer();

		//if its uninitialised return initial state
		if (buffer == 0) return AL_INITIAL;

		//get the state
		alGetSourcei(buffer, AL_SOURCE_STATE, &state);


		if (state == AL_INITIAL)
		{
			//DLOG("initial state");
			return AL_INITIAL;
		}

		if (state == AL_PLAYING)
		{
			//DLOG("Playing state");
			return AL_PLAYING;
		}

		if (state == AL_PAUSED)
		{
			//DLOG("Paused state");
			return AL_PAUSED;
		}

		if (state == AL_STOPPED)
		{
			//DLOG("Stopped state");
			return AL_STOPPED;
		}
	}

	void AudioComponent::updateState()
	{
		ALuint buffer = speaker->GetBuffer();
		alGetSourcei(buffer, AL_SOURCE_STATE, &state);

		getState();
	}
}
