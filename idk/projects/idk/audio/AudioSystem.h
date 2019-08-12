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
	//END Forward Declarations



	class AudioSystem : public ISystem
	{
	public:

		AudioSystem(); //Constructor
		~AudioSystem();//Destructor


		virtual void Init() override; //Initializes the FMOD Core System

		void Run();
		virtual void Shutdown() override;

		void SetMemoryAllocators(FMOD_MEMORY_ALLOC_CALLBACK useralloc, FMOD_MEMORY_REALLOC_CALLBACK userrealloc, FMOD_MEMORY_FREE_CALLBACK userfree); //Optional. Must be called before Initialize()

		void Initialize(); //Starts up a window for rendering

		///////////////////////////////////////////////
		//Sound Functions
		AudioClip* CreateSound(std::string filePath); //Creates a sound and returns a handle to the Sound. Do not handle deletion of KSound outside of KAudioEngine
		void DeleteSound(AudioClip** soundPointer); //Destroys sound, removes from map and nulls the soundpointer
		void AssignSoundToChannelGroup(); //Assigns sound to a channelgroup.
		void GetNumberOfSounds(int* i); //Gets number of sounds.


		//Project Functions
		void CreateChannelGroup(); //Creates a channel for a Sound to be in. Returns a pointer to the channel. This will be visible in the Project Settings
		void DeleteChannelGroup(); //Deletes a channel by pointer.
		void GetNumberOfChannelGroups(int* i); //Gets the number of channels in the Project


		//Serialization Functions TODO (Currently UNUSED)
		//void SaveLevelData(); //Saves/Load audio settings and preference of the level to file
		//void LoadLevelData(); //Loads sounds, then assigns to channelgroup, and updates volume, pitch etc.
		//
		//void SaveProjectData(); //Saves/Load audio settings and preference of the project to file. 
		//void LoadProjectData(); //It loads channelsgroups, number of max channels.


		//Get Data Functions
		float GetCPUPercentUsage();
		AUDIOSYSTEM_CPUDATA GetDetailedCPUPercentUsage();

		seconds GetTimeElapsedSinceInitialize() const;

		//std::cout function calls.
		string PrintTimeInitialized() const;
		string PrintTimeElapsed() const;
		void PrintMemoryStats(bool precise = true);		  //Todo, change to return a string instead

		//Helper functions
		static const char* FMOD_SOUND_TYPE_TO_C_STR(FMOD_SOUND_TYPE);
		static const char* FMOD_SOUND_FORMAT_TO_C_STR(FMOD_SOUND_FORMAT);

	private:
		FMOD::System* CoreSystem; //Is updated on init, destroyed and nulled on shutdown.
		FMOD_RESULT result;		//Most recent result by the most recent FMOD function call.
		int numberOfDrivers;	//Updated on init
		int currentDriver;		//Updated on init

		std::list<AudioClip*> AudioClipList; //Will be switched to a more faster container TODO

		time_point timeItWasInitialized;

		void ParseFMOD_RESULT(FMOD_RESULT); //All fmod function returns an FMOD_RESULT. This function parses the result. Throws EXCEPTION_AudioSystem if a function fails.




	};


	//Exception Handler
	struct EXCEPTION_AudioSystem {
		std::string exceptionDetails;
	};

	//Miscellaneous Data
	struct AUDIOSYSTEM_CPUDATA {
		float dsp;
		float stream;
		float geometry;
		float update;
		float total;
	};

}