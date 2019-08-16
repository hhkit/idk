//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSystem.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	
/*
	NOTES TO ME:
	ChannelGroups are multiplicative; A volume of 0.5 in group and 0.5 in individual channel equals to 0.25.
	You'd want to create all the audio for playing first before starting the level.
*/


//////////////////////////////////////////////////////////////////////////////////



#pragma once

#include <core/ISystem.h>
#include <idk.h>

#include "FMOD/core/fmod_common.h" //FMOD Enums. This is included in the header file only because this is the only thing that should be exposed.
#include <map> //map
#include <list> //list. TEMPORARY TODO. Will switch to a faster container.

//External Forward Declarations
namespace FMOD {
	class System; //CoreSystem
	class Sound;  //Sound
	class ChannelGroup;  //ChannelGroup (Sounds are played into a channel when play is called.)
	class SoundGroup;	 //SoundGroup	(Different from ChannelGroup, this is where the sound resides when create_sound is called.)
}
//END Forward Declarations

namespace idk
{

	//Forward Declarations
	class AudioClip;
	struct EXCEPTION_AudioSystem;
	struct AUDIOSYSTEM_CPUDATA;
	struct AUDIOSYSTEM_DRIVERDATA;
	//END Forward Declarations

	

	class AudioSystem : public ISystem
	{
	public:


		//SYSTEM CALLS
		///////////////////////////////////////////////
		AudioSystem(); //Constructor
		~AudioSystem();//Destructor

		virtual void Init() override; //Initializes the FMOD Core System
		void Run();
		virtual void Shutdown() override;

		//Optional. Must be called before Init()
		void SetMemoryAllocators(FMOD_MEMORY_ALLOC_CALLBACK useralloc, FMOD_MEMORY_REALLOC_CALLBACK userrealloc, FMOD_MEMORY_FREE_CALLBACK userfree); 

		///////////////////////////////////////////////



		//Sound Functions used by AudioSource
		//AudioClip* CreateAudioClip(string filePath, AudioClip::SubSoundGroup sndGrp = SubSoundGroup_SFX); //Creates a sound and returns a handle to the Sound. Do not handle deletion of KSound outside of KAudioEngine
		//void DeleteAudioClip(AudioClip*& soundPointerRef);			//Destroys sound, removes from map and nulls the soundpointer
		//void PlayAudioClip(AudioClip*& soundPointerRef);			//Plays a sound of an audioclip


		//Project Functions (Currently UNUSED)
		//void CreateChannelGroup(); //Creates a channel for a Sound to be in. Returns a pointer to the channel. This will be visible in the Project Settings
		//void DeleteChannelGroup(); //Deletes a channel by pointer.
		//void GetNumberOfChannelGroups(int* i); //Gets the number of channels in the Project


		//Serialization Functions (Currently UNUSED)
		//void SaveLevelData(); //Saves/Load audio settings and preference of the level to file
		//void LoadLevelData(); //Loads sounds, then assigns to channelgroup, and updates volume, pitch etc.
		//
		//void SaveProjectData(); //Saves/Load audio settings and preference of the project to file. 
		//void LoadProjectData(); //It loads channelsgroups, number of max channels.

		//Get Data Functions
		///////////////////////////////////////////////
		float							GetCPUPercentUsage();					//Gets the CPU usage in that tick.
		AUDIOSYSTEM_CPUDATA				GetDetailedCPUPercentUsage();			//Gets the CPU usage in that tick.
		vector<AUDIOSYSTEM_DRIVERDATA>	GetAllSoundDriverData() const;			//Gets Sound Drivers in the computer. This is empty if there are no available sound drivers!
		int								GetCurrentSoundDriverIndex() const;		//Because it is in index form, this returns the index of the sound driver that is running. If there is no available, -1 is returned.
		time_point						GetTimeInitialized() const;				//GetTime 
		void							GetMemoryStats(int* currentBytesAllocated, int* maxBytesAllocated, bool precise = true); //The pointer ints are modified to give the new byte number at point of call.
		///////////////////////////////////////////////


		//Helper functions
		///////////////////////////////////////////////
		static const char* FMOD_SOUND_TYPE_TO_C_STR(FMOD_SOUND_TYPE);
		static const char* FMOD_SOUND_FORMAT_TO_C_STR(FMOD_SOUND_FORMAT);
		static const char* FMOD_SPEAKERMODE_TO_C_STR(FMOD_SPEAKERMODE);
		///////////////////////////////////////////////

	private:
		friend class AudioClip;
		friend class AudioClipFactory;
		FMOD::System* CoreSystem;	//Is updated on init, destroyed and nulled on shutdown.
		static FMOD_RESULT result;			//Most recent result by the most recent FMOD function call.

		int numberOfDrivers;		//Updated on init. Describes the number of available sound driver that can play audio.
		int currentDriver;			//Updated on init. Describes the current running sound driver.

		vector<AudioClip*> AudioClipList;				//
		vector<AUDIOSYSTEM_DRIVERDATA> driverDetails;	//Describes each driver.

		time_point timeItWasInitialized;				//Updated on Init()

		//Useable after calling Init().
		FMOD::SoundGroup* soundGroup_MASTER;	//All sounds when created start at MASTER
		FMOD::SoundGroup* soundGroup_MUSIC;		//Music, by default is looped.
		FMOD::SoundGroup* soundGroup_SFX;		//Sound Effects
		FMOD::SoundGroup* soundGroup_AMBIENT;	//Ambient Sounds. This is similar to SFX. It is also OPTIONAL.
		FMOD::SoundGroup* soundGroup_DIALOGUE;	//Dialogue/Voice Overs. This is OPTIONAL.

		static void ParseFMOD_RESULT(FMOD_RESULT); //All fmod function returns an FMOD_RESULT. This function parses the result. Throws EXCEPTION_AudioSystem if a function fails.

	};


	//Exception Handler
	struct EXCEPTION_AudioSystem {
		string exceptionDetails;
	};

	//Miscellaneous Data
	//These floats range from 0 to 100 (percent)
	struct AUDIOSYSTEM_CPUDATA {
		AUDIOSYSTEM_CPUDATA() 
			:dsp{ 0 }, stream{ 0 }, geometry{ 0 }, update{ 0 }, total{ 0 }
		{}
		float dsp;		//DSP Mixing engine CPU usage
		float stream;	//Streaming engine CPU usage
		float geometry; //Geometry engine CPU usage
		float update;	//System::update CPU usage
		float total;	//Total CPU usage
	};

	struct AUDIOSYSTEM_DRIVERDATA {
		AUDIOSYSTEM_DRIVERDATA() 
			: driverIndex{ -1 }
			, driverName{ }
			, fmodID{}
			, systemRate{}
			, speakerMode{}
			, speakerModeChannels{}
		{}
		int driverIndex;
		char driverName[512];
		FMOD_GUID fmodID;
		int systemRate;
		FMOD_SPEAKERMODE speakerMode; //Use FMOD_SPEAKERMODE
		int speakerModeChannels;
	};

}