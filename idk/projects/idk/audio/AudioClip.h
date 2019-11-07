//////////////////////////////////////////////////////////////////////////////////
//@file		AudioClip.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		25 OCT 2019
//@brief	A class that contains the data holding FMOD sounds. This interacts with the AudioSystem directly. Not to be confused with AudioSource, which is a component that holds settings to the sound
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <res/Resource.h>
#include <util/enum.h>
#include "audio/AudioClipInfo.h"
#include "FMOD/core/fmod_common.h" //FMOD Enums. This is included in the header file only because this is the only thing that should be exposed.


//External Forward Declarations
namespace FMOD {
	class Sound;	//Sound
	class Channel;  //Channel
}
//END Forward Declarations

namespace idk
{

	ENUM (SubSoundGroup, int,
		SubSoundGroup_MUSIC,
		SubSoundGroup_SFX,
		SubSoundGroup_AMBIENT,
		SubSoundGroup_DIALOGUE
	)

	class AudioClip
		: public Resource<AudioClip>
	{ //A class that contains the data holding FMOD sounds. This interacts with the AudioSystem directly. Not to be confused with AudioSource
	public:


		AudioClip();
		~AudioClip();

		void	Play(); //If audio is not unique, it will duplicate another sound to play. Else, it will stop and replay a new sound. By default, will stop current sound and replay.
		void	Stop();

		void UpdateChannel(); //Updates the channel to null if it is not playing. It's important to update the channel before doing anything to it.

		//UpdateChannel first before calling all these
		void UpdateFmodMode(FMOD_MODE mode);

		void	SetIsPaused(bool i);
		bool	GetIsPaused();
		bool	GetIsPlaying();
		void	UpdateVolume(float i);
		void	UpdatePitch(float i);
		void	UpdatePriority(int i);
		void	UpdateMinMaxDistance(float min, float max);

		void	ReassignSoundGroup(SubSoundGroup newSndGrp); //Reassigns sound to a new soundgroup.

		AudioClipInfo GetAudioClipInfo();	//Returns a readonly information of the sound.
		string GetName();

	private:
		friend class AudioSystem;			//The AudioSystem will have access to AudioClip's variables
		friend class AudioClipLoader;		//The AudioClipLoader will have access to AudioClip's variables

		FMOD::Sound*	_soundHandle	{ nullptr };	//A handle to FMOD_Sound object. It contains some sound info data as well, but it is wrapped to the AudioClipInfo on CreateSound.
		FMOD::Channel*	_soundChannel	{ nullptr };	//Whenever a sound is played, this pointer becomes valid

		AudioClipInfo soundInfo  {}; //Updated by AudioClipLoader

		float	frequency	{ 44100.0f };	//Playback frequency. default = 44100	 					 //These are not saved, rather it is controlled by which SoundGroup it is at. 
		bool	isPlaying	{ false };	//Is the audio currently playing? If the audio is paused, it is still considered playing!
		int		priority	{ 128 };	//0 (most important) to 256 (least important) default = 128	 //These are not saved, rather it is controlled by which SoundGroup it is at. 

	};

}