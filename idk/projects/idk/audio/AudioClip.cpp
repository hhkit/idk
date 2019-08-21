//////////////////////////////////////////////////////////////////////////////////
//@file		AudioClip.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	A class that contains the data holding FMOD sounds. This interacts 
//			with the AudioSystem directly. Not to be confused with AudioSource, 
//			which is a component.

//////////////////////////////////////////////////////////////////////////////////


#pragma once
#include "stdafx.h" //Needed for every CPP. Precompiler
#include "audio/AudioClip.h" //AudioClip
#include "audio/AudioSystem.h" //AudioClip
//Dependency includes
#include <FMOD/core/fmod.hpp> //FMOD Core
#include <FMOD/core/fmod_errors.h> //ErrorString
namespace idk
{
	AudioClip::AudioClip()
	{
	}

	AudioClip::~AudioClip()
	{
		if (_soundHandle) {
			AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
			audioSystem.ParseFMOD_RESULT(_soundHandle->release());
		}
		_soundHandle = nullptr;
		_soundChannel = nullptr;
	}

	void AudioClip::Play() //Needs to update volume, pitch, priority
	{
		auto& audioSystem = Core::GetSystem<AudioSystem>();
		audioSystem.ParseFMOD_RESULT(audioSystem._Core_System->playSound(_soundHandle, nullptr, false, &_soundChannel)); //Creates a channel for audio to use. Start as paused to edit stuff first.
		SetPriority(priority);
		SetVolume(volume);
		SetPitch(pitch);
		
	}

	void AudioClip::Stop()
	{
		if (_soundChannel) {
			AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
			audioSystem.ParseFMOD_RESULT(_soundChannel->stop()); //Stop and forget.
			_soundChannel = nullptr;
		}
	}

	void AudioClip::SetIsPaused(bool i)
	{
		UpdateChannel();
		if (_soundChannel) {
			AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
			audioSystem.ParseFMOD_RESULT(_soundChannel->setPaused(i));
		}
	}

	bool AudioClip::GetIsPaused()
	{
		bool returnVal = false;
		UpdateChannel();
		if (_soundChannel) {
			AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
			audioSystem.ParseFMOD_RESULT(_soundChannel->getPaused(&returnVal));
		}
		return returnVal;
	}

	void AudioClip::SetVolume(float i)
	{
		volume = i;
		UpdateChannel();
		if (_soundChannel) {
			AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
			audioSystem.ParseFMOD_RESULT(_soundChannel->setVolume(volume));
		}
	}

	float AudioClip::GetVolume() const
	{
		return volume;
	}

	void AudioClip::SetPitch(float i)
	{
		pitch = i;
		UpdateChannel();
		if (_soundChannel) {
			AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
			audioSystem.ParseFMOD_RESULT(_soundChannel->setPitch(pitch));
		}
	}

	float AudioClip::GetPitch() const
	{
		return pitch;
	}

	void AudioClip::SetPriority(int i)
	{
		priority = i;
		UpdateChannel();
		if (_soundChannel) {
			AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
			audioSystem.ParseFMOD_RESULT(_soundChannel->setPriority(priority));
		}
	}

	int AudioClip::GetPriority() const
	{
		return priority;
	}

	void AudioClip::SetIsLoop(bool i)
	{
		isLoop = i;
		UpdateFmodMode();

	}

	bool AudioClip::GetIsLoop() const
	{
		return isLoop;
	}

	void AudioClip::SetIsUnique(bool i)
	{
		isUnique = i;
		UpdateFmodMode();

	}

	bool AudioClip::GetIsUnique() const
	{
		return isUnique;
	}

	void AudioClip::SetIs3DSound(bool i)
	{
		is3Dsound = i;
		UpdateFmodMode();

	}

	bool AudioClip::GetIs3DSound() const
	{
		return is3Dsound;
	}

	void AudioClip::UpdateChannel()
	{
		if (_soundChannel) { //check if it is a nullptr or not
			AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
			FMOD::Sound* check;
			//In the event that there is a LOT of sounds and channels are stolen, this checks if the channel is still its own.
			audioSystem.ParseFMOD_RESULT(_soundChannel->getCurrentSound(&check));
			char name[512];
			check->getName(name,512);
			//printf(name,"%s\n");
			if (check == _soundHandle) { //check returns null if no sound in that channel is playing.
				audioSystem.ParseFMOD_RESULT(_soundChannel->isPlaying(&isPlaying));
				if (isPlaying == false) {
					_soundChannel = nullptr; //Forget the channel once it is done playing
					//printf("Channel finished playing\n");

				}
				//printf("Channel still playing\n");

			}
			else {
				isPlaying = false;
				_soundChannel = nullptr; //Forget the channel if it is not playing anymore
				//printf("Channel stolen! Nulling Sound Channel\n");
			}
		}
		else {
			isPlaying = false;
			//printf("Channel is null\n");

		}
	}

	void AudioClip::UpdateFmodMode()
	{
		AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
		FMOD_MODE fmodMode = ConvertSettingToFMOD_MODE();
		audioSystem.ParseFMOD_RESULT(_soundHandle->setMode(fmodMode));

		UpdateChannel();
		if (_soundChannel) {
			audioSystem.ParseFMOD_RESULT(_soundHandle->setMode(fmodMode));
		}
	}



	void AudioClip::ReassignSoundGroup(SubSoundGroup newSndGrp)
	{
		auto& audioSystem = Core::GetSystem<AudioSystem>();

		switch (newSndGrp) {
		default:
		case SubSoundGroup_SFX:
			audioSystem.ParseFMOD_RESULT(_soundHandle->setSoundGroup(audioSystem._soundGroup_SFX));
			priority = 128;
			audioSystem.ParseFMOD_RESULT(_soundHandle->setDefaults(frequency, priority));
			break;

		case SubSoundGroup_MUSIC:
			audioSystem.ParseFMOD_RESULT(_soundHandle->setSoundGroup(audioSystem._soundGroup_MUSIC));
			priority = 32; //SFX has the lowest priority
			audioSystem.ParseFMOD_RESULT(_soundHandle->setDefaults(frequency, priority));
			break;

		case SubSoundGroup_AMBIENT:
			audioSystem.ParseFMOD_RESULT(_soundHandle->setSoundGroup(audioSystem._soundGroup_AMBIENT));
			priority = 32; //SFX has the lowest priority
			audioSystem.ParseFMOD_RESULT(_soundHandle->setDefaults(frequency, priority));
			break;

		case SubSoundGroup_DIALOGUE:
			audioSystem.ParseFMOD_RESULT(_soundHandle->setSoundGroup(audioSystem._soundGroup_DIALOGUE));
			priority = 64; //SFX has the lowest priority
			audioSystem.ParseFMOD_RESULT(_soundHandle->setDefaults(frequency, priority));
			break;
		}
	}

	AudioClipInfo AudioClip::GetAudioClipInfo()
	{
		return soundInfo;
	}
	FMOD_MODE AudioClip::ConvertSettingToFMOD_MODE()
	{
		FMOD_MODE output = FMOD_DEFAULT;
		output |= isLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		output |= is3Dsound ? FMOD_3D : FMOD_2D;
		if (isUnique) {
			output |= FMOD_UNIQUE;
		}
		return output;
	}
}