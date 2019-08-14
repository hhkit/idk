//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSystem.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	
//////////////////////////////////////////////////////////////////////////////////



#pragma once

#include <core/ISystem.h>
#include <idk.h>

#include "FMOD/core/fmod_common.h" //FMOD Enums. This is included in the header file only because this is the only thing that should be exposed.
#include <chrono> //Time
#include <string> //string
#include <map> //map
#include <list> //list. TEMPORARY TODO. Will switch to a faster container.

//External Forward Declarations
namespace FMOD {
	class System; //CoreSystem
	class Sound;  //Sound
}
//END Forward Declarations

namespace idk
{

	//Additional Forward Declarations
	class AudioClip;
	struct EXCEPTION_AudioSystem;
	struct AUDIOSYSTEM_CPUDATA;
	struct AUDIOSYSTEM_DRIVERDATA;
	//END Forward Declarations

	

	class AudioSystem : public ISystem
	{
	public:
		enum SubChannelGroup {
			SubChannelGroup_MUSIC,		//By default is looped
			SubChannelGroup_SFX,	
			SubChannelGroup_AMBIENT,	//By default is looped
			SubChannelGroup_DIALOGUE
		};

		AudioSystem(); //Constructor
		~AudioSystem();//Destructor


		virtual void Init() override; //Initializes the FMOD Core System
		void Run();
		virtual void Shutdown() override;

		//Optional. Must be called before Init()
		void SetMemoryAllocators(FMOD_MEMORY_ALLOC_CALLBACK useralloc, FMOD_MEMORY_REALLOC_CALLBACK userrealloc, FMOD_MEMORY_FREE_CALLBACK userfree); 

		///////////////////////////////////////////////
		//Sound Functions
		AudioClip* CreateSound(string filePath, SubChannelGroup chnGrp = SubChannelGroup_SFX); //Creates a sound and returns a handle to the Sound. Do not handle deletion of KSound outside of KAudioEngine
		void DeleteSound(AudioClip** soundPointer); //Destroys sound, removes from map and nulls the soundpointer
		void AssignSoundToChannelGroup(); //Assigns sound to a channelgroup.
		void GetNumberOfSounds(int* i); //Gets number of sounds.


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
		float GetCPUPercentUsage();	//Gets the CPU usage in that tick.
		AUDIOSYSTEM_CPUDATA GetDetailedCPUPercentUsage(); //Gets the CPU usage in that tick.
		vector<AUDIOSYSTEM_DRIVERDATA> GetAllSoundDriverData();
		int GetCurrentSoundDriverIndex();
		time_point GetTimeInitialized() const;

		//The pointer ints are modified to give the new byte number at point of call.
		void GetMemoryStats(int* currentBytesAllocated, int* maxBytesAllocated, bool precise = true);	

		//Helper functions
		static const char* FMOD_SOUND_TYPE_TO_C_STR(FMOD_SOUND_TYPE);
		static const char* FMOD_SOUND_FORMAT_TO_C_STR(FMOD_SOUND_FORMAT);
		static const char* FMOD_SPEAKERMODE_TO_C_STR(FMOD_SPEAKERMODE);

	private:
		FMOD::System* CoreSystem; //Is updated on init, destroyed and nulled on shutdown.
		FMOD_RESULT result;		//Most recent result by the most recent FMOD function call.
		int numberOfDrivers;	//Updated on init. Describes the number of available sound driver that can play audio.
		int currentDriver;		//Updated on init. Describes the current running sound driver.

		vector<AudioClip*> AudioClipList; //Will be switched to a more faster container TODO
		vector<AUDIOSYSTEM_DRIVERDATA> driverDetails; //Describes each driver.
		time_point timeItWasInitialized;

		void ParseFMOD_RESULT(FMOD_RESULT); //All fmod function returns an FMOD_RESULT. This function parses the result. Throws EXCEPTION_AudioSystem if a function fails.


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