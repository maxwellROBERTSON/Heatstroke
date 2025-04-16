#include "AudioComponent.hpp"
#include "../Audio/SoundDevice.h"
#include "../Audio/SoundBuffer.h"
#include "../Audio/SoundSource.h"


Engine::AudioComponent::AudioComponent()
{
	std::cout << "AudioComponent created" << std::endl;
	soundDevice = SoundDevice::get();
	soundBuffer = SoundBuffer::get();
	speaker = new SoundSource();
	soundClipCurrentlyPlaying = AL_INITIAL;
	state = AL_INITIAL;
}

void Engine::AudioComponent::GetDataArray(uint8_t* data)
{
	uint32_t offset = 0;
	
	std::memcpy(data + offset, &soundDevice, sizeof(soundDevice));
	offset += sizeof(soundDevice);
	std::memcpy(data + offset, &speaker, sizeof(speaker));
	offset += sizeof(speaker);
	std::memcpy(data + offset, &soundBuffer, sizeof(soundBuffer));
	offset += sizeof(soundBuffer);
	std::memcpy(data + offset, &soundClipCurrentlyPlaying, sizeof(soundClipCurrentlyPlaying));
	offset += sizeof(soundClipCurrentlyPlaying);
}

void Engine::AudioComponent::SetDataArray(uint8_t* data)
{
	uint32_t offset = 0;

	std::memcpy(&soundDevice, data + offset, sizeof(soundDevice));
	offset += sizeof(soundDevice);
	std::memcpy(&speaker, data + offset, sizeof(speaker));
	offset += sizeof(speaker);
	std::memcpy(&soundBuffer, data + offset, sizeof(soundBuffer));
	offset += sizeof(soundBuffer);
	std::memcpy(&soundClipCurrentlyPlaying, data + offset, sizeof(soundClipCurrentlyPlaying));
	offset += sizeof(soundClipCurrentlyPlaying);
}

void Engine::AudioComponent::AddClip(std::string key, std::string path)
{
	soundClips.insert({key, soundBuffer->get()->addSoundEffect(path.c_str())});
}

void Engine::AudioComponent::playSound(std::string key, bool interupt)
{
	getState();

	//is a clip currently playing?
	if (isPlaying() == false)
	{
		//no - set new active clip
		setActiveClip(key);

		//play new clip
		speaker->Play(soundClipCurrentlyPlaying);
	}
	else if (interupt == true)
	{
		//stop current clip
		speaker->Stop();

		std::cout << "Stopped file playing" << std::endl;

		//set new active clip
		setActiveClip(key);

		//play new clip
		speaker->Play(soundClipCurrentlyPlaying);
	}
	else
	{
		//yes - return
		std::cout << "Audio alreading playing through this source" << std::endl;
		return;
	}
}

bool Engine::AudioComponent::isPlaying()
{
	//update the state
	alGetSourcei(soundClipCurrentlyPlaying, AL_SOURCE_STATE, &state);
	getState();

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


void Engine::AudioComponent::setActiveClip(std::string key)
{
	soundClipCurrentlyPlaying = soundClips[key];
}


void Engine::AudioComponent::getState()
{
	if (state == AL_INITIAL) std::cout << "initial state\n";
	if (state == AL_PLAYING) std::cout << "Playing state\n";
	if (state == AL_PAUSED) std::cout << "Paused state\n";
	if (state == AL_STOPPED) std::cout << "Stopped state\n";
}
