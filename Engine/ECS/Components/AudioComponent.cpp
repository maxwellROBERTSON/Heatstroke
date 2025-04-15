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
	soundClip = -1;
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
	std::memcpy(data + offset, &soundClip, sizeof(soundClip));
	offset += sizeof(soundClip);
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
	std::memcpy(&soundClip, data + offset, sizeof(soundClip));
	offset += sizeof(soundClip);
}


void Engine::AudioComponent::AddClip(std::string path)
{
	soundClip = soundBuffer->get()->addSoundEffect(path.c_str());
}


//perform this on a separate thread?
void Engine::AudioComponent::playSound()
{
	speaker->Play(soundClip);
}


