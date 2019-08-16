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
	class Sound;  //Sound
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

		void Play();
		void Stop();
		void Pause();
		void Update(); //Updates volume, pitch etc.

		void ReassignSoundGroup(AudioClip* audioPointer, SubSoundGroup newSndGrp); //Reassigns sound to a new soundgroup.


		//DEBUG FUNCTIONS NEEDED
		AudioClipInfo GetAudioClipInfo(); //Returns a readonly information of the sound.
	private:
		friend class AudioSystem; //The AudioSystem will have access to AudioClip's variables
		friend class AudioClipFactory; //The AudioSystem will have access to AudioClip's variables
		AudioSystem* audioSystem; //Access to AudioSystem's public variables
		FMOD::Sound* soundHandle; //A handle to FMOD_Sound object. It contains some sound info data as well, but it is wrapped to the AudioClipInfo on CreateSound.

		AudioClipInfo soundInfo;
		float volume;
		float pitch; //Changing pitch will affect the length of the sound, but is not updated in the SoundInfo. The SoundInfo contains the raw data of it.
		//More stuff here
	};

}