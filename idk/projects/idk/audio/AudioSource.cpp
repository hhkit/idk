//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSource.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	A GameObject Component that holds AudioClips to play sounds.

//////////////////////////////////////////////////////////////////////////////////


#include <stdafx.h> //Needed for every CPP. Precompiler
#include <audio/AudioSource.h>
#include <audio/AudioClipFactory.h>
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
		RscHandle<AudioClip> audioPtr1 = Core::GetResourceManager().Create<AudioClip>(Core::GetSystem<FileSystem>().GetFile(filePath));

		audio_clip_list.push_back(audioPtr1);
	}


	void AudioSource::RemoveAudioClip(int index)
	{
		if (audio_clip_list.size() > index) { //Check if it is in array
			audio_clip_list.erase(audio_clip_list.begin() + index);
		}
	}
}