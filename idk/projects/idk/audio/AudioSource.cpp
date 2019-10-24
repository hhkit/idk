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
	void AudioSource::Play(int index)
	{
		if (audio_clip_list.size() > index) { //Check if it is in array
			audio_clip_list[index]->Play();
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
			i->Stop();
		}

	}

	void AudioSource::AddAudioClip(string_view filePath)
	{
		auto audioPtr1 = Core::GetResourceManager().Load<AudioClip>(Core::GetSystem<FileSystem>().GetFile(filePath));
		if (audioPtr1)
			audio_clip_list.push_back(*audioPtr1);
	}


	void AudioSource::RemoveAudioClip(int index)
	{
		if (audio_clip_list.size() > index) { //Check if it is in array
			audio_clip_list.erase(audio_clip_list.begin() + index);
		}
	}
	void AudioSource::UpdateAudioClips()
	{

		for (int i = 0; i < audio_clip_list.size(); ++i) {
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