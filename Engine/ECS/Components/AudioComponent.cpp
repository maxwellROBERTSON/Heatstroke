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

void Engine::AudioComponent::GetDataArray(uint8_t*)
{
}

void Engine::AudioComponent::SetDataArray(uint8_t*)
{
}


void Engine::AudioComponent::AddClip(std::string path)
{
	soundClip = soundBuffer->get()->addSoundEffect(path.c_str());
}


//perform this on a separate thread?
void Engine::AudioComponent::PlaySound()
{
	speaker->Play(soundClip);
}
