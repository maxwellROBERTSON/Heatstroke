#include "SoundSource.hpp"


SoundSource::SoundSource()
{
	//generates the number of sources inputted 
	alGenSources(1, &p_Source);

	//sets the properties of the object members
	alSourcef(p_Source, AL_PITCH, p_Pitch);
	alSourcef(p_Source, AL_GAIN, p_Volume);
	alSource3f(p_Source, AL_POSITION, p_Position[0], p_Position[1], p_Position[2]);
	alSource3f(p_Source, AL_VELOCITY, p_Velocity[0], p_Velocity[1], p_Velocity[2]);
	alSourcei(p_Source, AL_LOOPING, p_LoopSound);
	alSourcei(p_Source, AL_BUFFER, p_Buffer);
}

SoundSource::~SoundSource()
{
	//delete speaker
	alDeleteSources(1, &p_Source);
}

void SoundSource::Play(const ALuint bufferToPlay)
{
	//only upload new buffer if sound file has changed
	if (bufferToPlay != p_Buffer)
	{
		p_Buffer = bufferToPlay;
		//AL_BUFFER = the ID of the attached buffer
		alSourcei(p_Source, AL_BUFFER, p_Buffer);
	}

	//parameter is the name of the source to be played
	alSourcePlay(p_Source);
	//The playing source will have its state changed to AL_PLAYING. When called on a source 
	//which is already playing, the source will restart at the beginning.When the attached
	//buffer(s) are done playing, the source will progress to the AL_STOPPED state.
}