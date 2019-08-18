//////////////////////////////////////////////////////////////////////////////////
//@file		AudioClip.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	A class that contains the data holding FMOD sounds. This interacts with the AudioSystem directly. Not to be confused with AudioSource, which is a component
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <res/Resource.h>
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
	class AudioClip
		: public Resource<AudioClip>
	{ //A class that contains the data holding FMOD sounds. This interacts with the AudioSystem directly. Not to be confused with AudioSource
	public:

		enum SubSoundGroup {
			SubSoundGroup_MUSIC,
			SubSoundGroup_SFX,
			SubSoundGroup_AMBIENT,
			SubSoundGroup_DIALOGUE
		};

		AudioClip();
		~AudioClip();

		void Play(); //If audio is not unique, it will duplicate another sound to play. Else, it will stop and replay a new sound.
		void Stop();
		void Pause();
		void Unpause();

		void Update(); //Updates volume, pitch etc.

		void ReassignSoundGroup(SubSoundGroup newSndGrp); //Reassigns sound to a new soundgroup.

		AudioClipInfo GetAudioClipInfo();	//Returns a readonly information of the sound.

	private:
		friend class AudioSystem;			//The AudioSystem will have access to AudioClip's variables
		friend class AudioClipFactory;		//The AudioSystem will have access to AudioClip's variables

		FMOD::Sound*	_soundHandle	{ nullptr };	//A handle to FMOD_Sound object. It contains some sound info data as well, but it is wrapped to the AudioClipInfo on CreateSound.
		FMOD::Channel*	_soundChannel	{ nullptr };	//Whenever a sound is played, this pointer becomes valid

		AudioClipInfo soundInfo  {};


		float	volume		{ 1.0f   };
		float	pitch		{ 1.0f   };	//Changing pitch will affect the length of the sound, but is not updated in the SoundInfo. The SoundInfo contains the raw data of it.
		float	minDistance	{ 100.0f };	//Minimum distance where volume is at max
		int		priority	{ 128    };	//0 (most important) to 256 (least important) default = 128
		bool	playOnStart	{ false  };	//Does this sound play on start of load?
		bool	isPlaying	{ false  };	//Is the audio currently playing?
		bool	is3Dsound	{ true   };	//Does this sound follow the the gameobject position?
		bool	loop		{ false  };	//Does this audio loop?

	};

}