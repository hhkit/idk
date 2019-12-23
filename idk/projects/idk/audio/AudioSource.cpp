//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSource.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		08 NOV 2019
//@brief	A GameObject Component that holds AudioClips to play sounds. REFER TO PUSH ID: 6e680f1d FOR BACKUP

//////////////////////////////////////////////////////////////////////////////////


#include <stdafx.h> //Needed for every CPP. Precompiler
#include <audio/AudioSource.h>
#include <res/Resource.h>
#include <file/FileSystem.h>
#include <FMOD/core/fmod.hpp> //FMOD Core
#include <FMOD/core/fmod_errors.h> //ErrorString
#include <audio/AudioSystem.h> //AudioSystem
#include <res/ResourceManager.h>
#include <sstream> //AudioSystem

namespace idk
{
	AudioSource::~AudioSource()
	{
		StopAll();

	}
	void AudioSource::Play(int index)
	{
		if (audio_clip_list.size() > index) { //Check if it is in array
			if (!audio_clip_list[index])
				return;
			AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
			//Temporarily set the mode of the _soundHandle before returning back to original, so that the channel carries it
			//FMOD_MODE tempMode;
			//audio_clip_list[index]->_soundHandle->getMode(&tempMode);
			//audio_clip_list[index]->_soundHandle->setMode(ConvertSettingToFMOD_MODE());
			audioSystem.ParseFMOD_RESULT(audioSystem._Core_System->playSound(audio_clip_list[index]->_soundHandle, nullptr, false, &audio_clip_channels[index])); //Creates a channel for audio to use. Start as paused to edit stuff first.
			//audio_clip_list[index]->_soundHandle->setMode(tempMode);
		}
	}
	void AudioSource::PlayAll()
	{
		AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
		FMOD_MODE currentMode = ConvertSettingToFMOD_MODE();

		for (int i = 0; i < audio_clip_list.size(); ++i) {
			//FMOD_MODE tempMode;
			//audio_clip_list[i]->_soundHandle->getMode(&tempMode);
			audio_clip_list[i]->_soundHandle->setMode(currentMode);
			audioSystem.ParseFMOD_RESULT(audioSystem._Core_System->playSound(audio_clip_list[i]->_soundHandle, nullptr, false, &audio_clip_channels[i])); //Creates a channel for audio to use. Start as paused to edit stuff first.
			//audio_clip_list[i]->_soundHandle->setMode(tempMode);
		}
	}
	void AudioSource::Stop(int index)
	{
		if (audio_clip_list.size() > index) { //Check if it is in array
			if (audio_clip_channels[index] != nullptr) {
				AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
				bool checkIsPlaying = false;	//An invalid channel can still return an isplaying, use this to stop!
				audioSystem.ParseFMOD_RESULT(audio_clip_channels[index]->isPlaying(&checkIsPlaying));
				if (checkIsPlaying)
					audioSystem.ParseFMOD_RESULT(audio_clip_channels[index]->stop()); //Creates a channel for audio to use. Start as paused to edit stuff first.
				audio_clip_channels[index] = nullptr;
			}
		}
	}
	void AudioSource::StopAll()
	{
		AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();

		for (int i = 0; i < audio_clip_channels.size(); ++i) {
			if (audio_clip_channels[i] != nullptr) {
				//bool checkIsPlaying = false;	//An invalid channel can still return an isplaying, use this to stop!
				try {
					audioSystem.ParseFMOD_RESULT(audio_clip_channels[i]->stop());
				}
				catch (...) {}//FMOD_ERR_INVALID_HANDLE will appear here. 
				audio_clip_channels[i] = nullptr;
			}
		}

	}

	int AudioSource::AddAudioClip(string_view filePath)
	{
		auto audioPtr1 = Core::GetResourceManager().Load<AudioClip>(PathHandle(filePath));
		if (audioPtr1) {
			audio_clip_list.emplace_back(*audioPtr1);
			audio_clip_channels.push_back(nullptr);
			return static_cast<int>(audio_clip_list.size() - 1);
		}
		else
			return -1;
	}


	void AudioSource::RemoveAudioClip(int index)
	{
		if (audio_clip_list.size() > index) { //Check if it is in array
			Stop(index); //Stop first
			audio_clip_list.erase(audio_clip_list.begin() + index);
			audio_clip_channels.erase(audio_clip_channels.begin() + index);
		}
	}
	bool AudioSource::IsAudioClipPlaying(int index)
	{
		bool returnVal = false;

		if (audio_clip_channels.size() > index) { //Check if it is in array
			//Copied from AudioClip
			if (audio_clip_channels[index] == nullptr)
				return returnVal;

			audio_clip_channels[index]->isPlaying(&returnVal);
		}
		return returnVal;
	}
	bool AudioSource::IsAnyAudioClipPlaying()
	{
		bool returnVal = false;
		for (int i = 0; i < audio_clip_list.size(); ++i) {
			audio_clip_channels[i]->isPlaying(&returnVal);
			if (returnVal)
				return true;
		}
		return false;
	}
	void AudioSource::UpdateAudioClips()
	{
		FMOD_MODE mode = ConvertSettingToFMOD_MODE();
		if (audio_clip_list.size() != audio_clip_channels.size())
			audio_clip_channels.resize(audio_clip_list.size(), nullptr);

		for (int i = 0; i < audio_clip_list.size(); ++i) {
			if (!audio_clip_list[i])
				continue;
			//---UPDATE CHANNEL---
			if (audio_clip_channels[i] != nullptr) {
				bool isPlaying;
				FMOD_RES(audio_clip_channels[i]->isPlaying(&isPlaying));
				if (isPlaying == false) {
					audio_clip_channels[i] = nullptr; //Forget the channel once it is done playing
					//printf("Channel finished playing\n");
					continue; //Go to next audio
				}
			}
			else
				continue;
			FMOD_RES(audio_clip_channels[i]->setVolume(volume));
			pitch = pitch < 0 ? 0 : pitch;
			FMOD_RES(audio_clip_channels[i]->setPitch(pitch));
			FMOD_RES(audio_clip_channels[i]->set3DMinMaxDistance(minDistance, maxDistance));
			FMOD_RES(audio_clip_channels[i]->setMode(mode));

			//Update Priority
			//Update Position
		}

	}
	int AudioSource::FindAudio(string_view name)
	{
		for (int i = 0; i < audio_clip_list.size(); ++i) {
			const auto stringName = audio_clip_list[i]->GetName();
			if (stringName == name)
				return i;
		}
		return -1;
	}
	FMOD_MODE AudioSource::ConvertSettingToFMOD_MODE()
	{
		FMOD_MODE output = FMOD_DEFAULT | FMOD_3D_LINEARROLLOFF;
		output |= isLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
		output |= is3Dsound ? FMOD_3D : FMOD_2D;
		if (isUnique) {
			output |= FMOD_UNIQUE;
		}
		return output;
	}

	void AudioSource::FMOD_RES(FMOD_RESULT e) //Same as audiosystem ParseFMOD_RESULT, but does not update the _result variable in audiosystem
	{
		if (e != FMOD_OK)
			LOG_TO(LogPool::SYS, "FMOD error! (%d) %s ", e, FMOD_ErrorString(e));
	}
}