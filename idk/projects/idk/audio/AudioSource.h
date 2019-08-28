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

		void Play(int audioClip_index = 0);
		void Stop();

		void AddAudioClip(string_view filePath); //Calls to add audio clip with path given
		
		hash_table<string, RscHandle<AudioClip>> audio_clip_list;

		float	volume		{ 1.0f		};	//Default = 1 Range: [0,1]
		float	pitch		{ 1.0f		};	//Changing pitch will affect the length of the sound, but is not updated in the SoundInfo. The SoundInfo contains the raw data of it.
		float	minDistance	{ 1.0f		};	//Minimum distance where volume is at max. This is in meters					 
		float	maxDistance	{ 100.0f	};	//Maximum distance where i can hear the sound. This is in meters					  
		bool	isPlaying	{ false		};	//Is the audio currently playing? If the audio is paused, it is still considered playing!
		bool	is3Dsound	{ true		};	//Does this sound follow the the gameobject position?
		bool	isUnique	{ true		};	//When I call play, does it duplicate? Or replay the sound again?
		bool	isLoop		{ false		};	//Does this audio loop?


	private:



	};

}