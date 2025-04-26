#include "AudioComponent.hpp"
#include "../Audio/SoundDevice.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "../Audio/SoundSource.hpp"

namespace Engine
{
	AudioComponent::AudioComponent()
	{
		std::cout << "AudioComponent created" << std::endl;
		soundDevice = SoundDevice::get();
		soundBuffer = SoundBuffer::get();
		speaker = new SoundSource();
		soundClipCurrentlyPlaying = 0;
		state = AL_INITIAL;
	}

	void AudioComponent::GetDataArray(uint8_t* data)
	{
		uint32_t offset = 0;

		std::memcpy(data + offset, &soundClipCurrentlyPlaying, sizeof(soundClipCurrentlyPlaying));
		offset += sizeof(soundClipCurrentlyPlaying);
	}

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
			std::cout << "No audio clip with that key value exists\n";
			return;
		}

		getState();

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

		//	std::cout << "Stopped file playing" << std::endl;

		//	//set new active clip
		//	//setActiveClip(key);

		//	//play new clip
		//	speaker->Play(soundClipCurrentlyPlaying);

		//	updateState();
		//}
		//else
		//{
		//	//yes - return
		//	std::cout << "Audio alreading playing through this source" << std::endl;
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
			std::cout << "ERROR from alGetError() " << res << std::endl;

			if (res == ALC_NO_ERROR) std::cout << "ALC_NO_ERROR\n";
			if (res == ALC_INVALID_DEVICE) std::cout << "ALC_INVALID_DEVICE\n";
			if (res == ALC_INVALID_CONTEXT) std::cout << "ALC_INVALID_CONTEXT\n";
			if (res == ALC_INVALID_ENUM) std::cout << "ALC_INVALID_ENUM\n";
			if (res == ALC_INVALID_VALUE) std::cout << "ALC_INVALID_VALUE\n";
			if (res == ALC_OUT_OF_MEMORY) std::cout << "ALC_OUT_OF_MEMORY\n";
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
			std::cout << "initial state\n";
			return AL_INITIAL;
		}

		if (state == AL_PLAYING)
		{
			std::cout << "Playing state\n";
			return AL_PLAYING;
		}

		if (state == AL_PAUSED)
		{
			std::cout << "Paused state\n";
			return AL_PAUSED;
		}

		if (state == AL_STOPPED)
		{
			std::cout << "Stopped state\n";
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
