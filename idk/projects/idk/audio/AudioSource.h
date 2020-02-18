//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSource.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		08 NOV 2019
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

		int AddAudioClip(string_view filePath); //Calls to add audio clip with path given. Returns true if filepath succeeds
		void RemoveAudioClip(int index = 0); //Calls to add audio clip with path given
		//bool IsAudioPaused(int index)
		bool IsAudioClipPlaying(int index);
		bool IsAnyAudioClipPlaying();
		int  FindAudio(string_view name);

		void UpdateAudioClips();

		vector<RscHandle<AudioClip>> audio_clip_list;
		vector<float>				 audio_clip_volume;	//This is tightly updated with the list, like a pair
		vector<FMOD::Channel*>		 audio_clip_channels;	//This is tightly updated with the list, like a pair

		//All the Audio data is inside AudioClip

		float			volume		{ 1.0f };	//Default = 1 Range: [0,1]
		float			pitch		{ 1.0f };	//Changing pitch will affect the length of the sound, but is not updated in the SoundInfo. The SoundInfo contains the raw data of it.
		float			minDistance	{ 1.0f };	//Minimum distance where volume is at max. This is in meters					 
		float			maxDistance	{ 100.0f };	//Maximum distance where i can hear the sound. This is in meters					 
		bool			is3Dsound	{ true };	//Does this sound follow the the gameobject position?
		bool			isUnique	{ true };	//When I call play, does it duplicate? Or replay the sound again?
		bool			isLoop		{ false };	//Does this audio loop?
		//ADD PRIORITY HERE?
		SubSoundGroup	soundGroup	{ SubSoundGroup::SubSoundGroup_SFX };


		void ResizeAudioClipListData();
	private:
		FMOD_MODE ConvertSettingToFMOD_MODE(); //For FMOD::System.setMode. Collates the current setting given.


		void FMOD_RES(FMOD_RESULT e); //Throws string on fail
	};

}