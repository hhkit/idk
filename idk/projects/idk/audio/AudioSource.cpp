//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSource.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	A GameObject Component that holds AudioClips to play sounds.

//////////////////////////////////////////////////////////////////////////////////


#include <stdafx.h> //Needed for every CPP. Precompiler
#include <audio/AudioSource.h>
#include <res/Resource.h>
#include <file/FileSystem.h>

namespace idk
{
	AudioSource::~AudioSource()
	{
		StopAll();

	}
	void AudioSource::Play(int index)
	{
		if (audio_clip_list.size() > index) { //Check if it is in array
			audio_clip_list[index]->Play();
		}
	}
	void AudioSource::PlayAll()
	{
		for (auto& i : audio_clip_list) {
			if (i)
				i->Play();
		}
	}
	void AudioSource::Stop(int index)
	{
		if (audio_clip_list.size() > index) { //Check if it is in array
			audio_clip_list[index]->Stop();
		}
	}
	void AudioSource::StopAll()
	{
		for (auto& i : audio_clip_list) {
			if (i)
				i->Stop();
		}

	}

	void AudioSource::AddAudioClip(string_view filePath)
	{
		auto audioPtr1 = Core::GetResourceManager().Load<AudioClip>(Core::GetSystem<FileSystem>().GetFile(filePath));
		if (audioPtr1)
			audio_clip_list.emplace_back(*audioPtr1);
	}


	void AudioSource::RemoveAudioClip(int index)
	{
		if (audio_clip_list.size() > index) { //Check if it is in array
			audio_clip_list[index]->Stop(); //Stop first
			audio_clip_list.erase(audio_clip_list.begin() + index);
		}
	}
	bool AudioSource::IsAudioClipPlaying(int index)
	{
		return audio_clip_list[index]->GetIsPlaying();
	}
	bool AudioSource::IsAnyAudioClipPlaying()
	{
		for (int i = 0; i < audio_clip_list.size(); ++i) {
			if (audio_clip_list[i]->GetIsPlaying())
				return true;
		}
		return false;
	}
	void AudioSource::UpdateAudioClips()
	{

		for (int i = 0; i < audio_clip_list.size(); ++i) {
			if (!audio_clip_list[i])
				continue;
			//Update Channel to check if it is still playing.
			audio_clip_list[i]->UpdateChannel();
			//Update volume
			audio_clip_list[i]->UpdateVolume(volume);
			//update pitch
			audio_clip_list[i]->UpdatePitch(pitch);
			//Update MinMax Distance
			audio_clip_list[i]->UpdateMinMaxDistance(minDistance, maxDistance);
			//Update FMODMODE
			audio_clip_list[i]->UpdateFmodMode(ConvertSettingToFMOD_MODE());
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
}