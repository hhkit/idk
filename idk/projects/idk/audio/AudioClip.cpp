//////////////////////////////////////////////////////////////////////////////////
//@file		AudioClip.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	A class that contains the data holding FMOD sounds. This interacts 
//			with the AudioSystem directly. Not to be confused with AudioSource, 
//			which is a component. REFER TO PUSH ID: 6e680f1d FOR BACKUP

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
	}

	void AudioClip::ReassignSoundGroup(SubSoundGroup newSndGrp)
	{
		auto& audioSystem = Core::GetSystem<AudioSystem>();

		switch (newSndGrp) {
		default:
		case SubSoundGroup::SubSoundGroup_SFX:
			audioSystem.ParseFMOD_RESULT(_soundHandle->setSoundGroup(audioSystem._soundGroup_SFX));
			priority = 128;
			audioSystem.ParseFMOD_RESULT(_soundHandle->setDefaults(frequency, priority));
			break;

		case SubSoundGroup::SubSoundGroup_MUSIC:
			audioSystem.ParseFMOD_RESULT(_soundHandle->setSoundGroup(audioSystem._soundGroup_MUSIC));
			priority = 32; //SFX has the lowest priority
			audioSystem.ParseFMOD_RESULT(_soundHandle->setDefaults(frequency, priority));
			break;

		case SubSoundGroup::SubSoundGroup_AMBIENT:
			audioSystem.ParseFMOD_RESULT(_soundHandle->setSoundGroup(audioSystem._soundGroup_AMBIENT));
			priority = 32; //SFX has the lowest priority
			audioSystem.ParseFMOD_RESULT(_soundHandle->setDefaults(frequency, priority));
			break;

		case SubSoundGroup::SubSoundGroup_DIALOGUE:
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

	string AudioClip::GetName()
	{
		return soundInfo.name;
	}

}