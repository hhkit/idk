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
		: audioSystem	{nullptr}
		, soundHandle	{nullptr}
		, volume		{ 1.0f }
		, pitch			{ 1.0f }
	{
	}
	AudioClip::~AudioClip()
	{
		AudioSystem::ParseFMOD_RESULT(soundHandle->release());
		soundHandle = nullptr;
	}
	void AudioClip::Play()
	{
		FMOD::System* system;
		soundHandle->getSystemObject(&system);
		FMOD::Channel* channel;

		system->playSound(soundHandle, nullptr, false, &channel);

	}
	AudioClipInfo AudioClip::GetAudioClipInfo()
	{
		return soundInfo;
	}
}