//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSource.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	A GameObject Component that holds AudioClips to play sounds.
/*
			Starts off as no audio clip

*/
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <core/Component.h>


namespace idk
{
	class AudioSource :public Component<AudioSource> {
	public:

		void Play(int index = 0);
		void Stop(int index = 0);
		void StopAll();

		void AddAudioClip(string_view filePath); //Calls to add audio clip with path given
		void RemoveAudioClip(int index = 0); //Calls to add audio clip with path given
		
		vector<RscHandle<AudioClip>> audio_clip_list;

		//All the Audio data is inside AudioClip


	private:



	};

}