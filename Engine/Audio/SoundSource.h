#pragma once
#include "../third_party/AL/al.h"

//object that will play the sound
class SoundSource
{
public:
	SoundSource();
	~SoundSource();

	void Play(const ALuint bufferToPlay);

private:
	//speaker variables
	ALuint p_Source;
	float p_Pitch = 1.f;
	float p_Volume = 1.f; //volume
	float p_Position[3] = { 0,0,0 };
	float p_Velocity[3] = { 0,0,0 };
	bool p_LoopSound = false;
	ALuint p_Buffer = 0; 
};

