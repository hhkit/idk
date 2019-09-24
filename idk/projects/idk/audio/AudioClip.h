//////////////////////////////////////////////////////////////////////////////////
//@file		AudioClip.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	A class that contains the data holding FMOD sounds. This interacts with the AudioSystem directly. Not to be confused with AudioSource, which is a component
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

	struct AudioMeta {
		float	volume{ 1.0f };	//Default = 1 Range: [0,1]
		float	pitch{ 1.0f };	//Changing pitch will affect the length of the sound, but is not updated in the SoundInfo. The SoundInfo contains the raw data of it.
		float	minDistance{ 1.0f };	//Minimum distance where volume is at max. This is in meters					 
		float	maxDistance{ 100.0f };	//Maximum distance where i can hear the sound. This is in meters					 
		bool	is3Dsound{ true };	//Does this sound follow the the gameobject position?
		bool	isUnique{ true };	//When I call play, does it duplicate? Or replay the sound again?
		bool	isLoop{ false };	//Does this audio loop?

	};


	class AudioClip
		: public Resource<AudioClip>,
		  public MetaTag<AudioMeta>
	{ //A class that contains the data holding FMOD sounds. This interacts with the AudioSystem directly. Not to be confused with AudioSource
	public:


		AudioClip();
		~AudioClip();

		void	Play(); //If audio is not unique, it will duplicate another sound to play. Else, it will stop and replay a new sound. By default, will stop current sound and replay.
		void	Stop();

		void	SetIsPaused(bool i);
		bool	GetIsPaused();
		void	SetVolume(float i);
		float	GetVolume() const;
		void	SetPitch(float i);
		float	GetPitch() const;
		void	SetPriority(int i);
		int		GetPriority() const;
		void	SetIsLoop(bool i);
		bool	GetIsLoop() const;
		void	SetIsUnique(bool i);
		bool	GetIsUnique() const;
		void	SetIs3DSound(bool i);
		bool	GetIs3DSound() const;
		void	SetMinDistance(float i);
		float	GetMinDistance() const;
		void	SetMaxDistance(float i);
		float	GetMaxDistance() const;

		void ReassignSoundGroup(SubSoundGroup newSndGrp); //Reassigns sound to a new soundgroup.

		AudioClipInfo GetAudioClipInfo();	//Returns a readonly information of the sound.

	private:
		friend class AudioSystem;			//The AudioSystem will have access to AudioClip's variables
		friend class AudioClipLoader;		//The AudioClipLoader will have access to AudioClip's variables

		FMOD::Sound*	_soundHandle	{ nullptr };	//A handle to FMOD_Sound object. It contains some sound info data as well, but it is wrapped to the AudioClipInfo on CreateSound.
		FMOD::Channel*	_soundChannel	{ nullptr };	//Whenever a sound is played, this pointer becomes valid

		AudioClipInfo soundInfo  {};

		float	frequency{ 44100.0f };	//Playback frequency. default = 44100	 					 //These are not saved, rather it is controlled by which SoundGroup it is at. 
		int		priority{ 128 };	//0 (most important) to 256 (least important) default = 128	 //These are not saved, rather it is controlled by which SoundGroup it is at. 
		bool	isPlaying{ false };	//Is the audio currently playing? If the audio is paused, it is still considered playing!



		FMOD_MODE ConvertSettingToFMOD_MODE(); //For FMOD::System.setMode. Collates the current setting given.
		void UpdateChannel(); //Updates the channel to null if it is not playing. It's important to update the channel before doing anything to it.
		void UpdateFmodMode(); //A wrapper.
		void UpdateMinMaxDistance(); //A wrapper.


		virtual void OnMetaUpdate(const AudioMeta& newmeta) override;
	};

}