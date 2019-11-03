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
#include <audio/AudioClip.h>


namespace idk
{
	class AudioSource :public Component<AudioSource>{
	public:

		~AudioSource();

		void Play(int index = 0);
		void PlayAll();
		void Stop(int index = 0);
		void StopAll();

		void AddAudioClip(string_view filePath); //Calls to add audio clip with path given
		void RemoveAudioClip(int index = 0); //Calls to add audio clip with path given
		
		bool IsAudioClipPlaying(int index);
		bool IsAnyAudioClipPlaying();
		void UpdateAudioClips();
		vector<RscHandle<AudioClip>> audio_clip_list;

		//All the Audio data is inside AudioClip

		float			volume		{ 1.0f };	//Default = 1 Range: [0,1]
		float			pitch		{ 1.0f };	//Changing pitch will affect the length of the sound, but is not updated in the SoundInfo. The SoundInfo contains the raw data of it.
		float			minDistance	{ 1.0f };	//Minimum distance where volume is at max. This is in meters					 
		float			maxDistance	{ 100.0f };	//Maximum distance where i can hear the sound. This is in meters					 
		bool			is3Dsound	{ true };	//Does this sound follow the the gameobject position?
		bool			isUnique	{ true };	//When I call play, does it duplicate? Or replay the sound again?
		bool			isLoop		{ false };	//Does this audio loop?
		SubSoundGroup	soundGroup	{ SubSoundGroup::SubSoundGroup_SFX };


	private:
		FMOD_MODE ConvertSettingToFMOD_MODE(); //For FMOD::System.setMode. Collates the current setting given.



	};

}