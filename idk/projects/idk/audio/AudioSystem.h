//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSystem.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	
/*
	NOTES TO ME:
	ChannelGroups are multiplicative; A volume of 0.5 in group and 0.5 in individual channel equals to 0.25.
	You'd want to create all the audio for playing first before starting the level.
	For simplicity sake, the mixes (MASTER, SFX, MUSIC etc) are called Channels. 
	(They are not the same as FMOD::Channel, but you don't need to know that.)
*/


//////////////////////////////////////////////////////////////////////////////////



#pragma once

#include <core/ISystem.h>
#include <idk.h>

#include "FMOD/core/fmod_common.h" //FMOD Enums. This is included in the header file only because this is the only thing that should be exposed.
#include <core/Handle.h>

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
		void Update(span<AudioSource>, span<AudioListener>);
		void UpdateTestCaseOnly(); //Remove after test!
		void StopAllAudio();

		void SetSystemPaused(bool);


		virtual void Shutdown() override;

		//Optional. Must be called before Init()
		void SetMemoryAllocators(FMOD_MEMORY_ALLOC_CALLBACK useralloc, FMOD_MEMORY_REALLOC_CALLBACK userrealloc, FMOD_MEMORY_FREE_CALLBACK userfree); 
		///////////////////////////////////////////////

		//Sound Functions used by AudioSource
		//AudioClip* CreateAudioClip(string filePath, AudioClip::SubSoundGroup sndGrp = SubSoundGroup_SFX); //Creates a sound and returns a handle to the Sound. Do not handle deletion of KSound outside of KAudioEngine
		//void DeleteAudioClip(AudioClip*& soundPointerRef);		//Destroys sound, removes from map and nulls the soundpointer
		//void PlayAudioClip(AudioClip*& soundPointerRef);			//Plays a sound of an audioclip

		//Channel Controls
		///////////////////////////////////////////////
		void  SetChannel_MASTER_Volume(const float& newVolume);
		float GetChannel_MASTER_Volume();
		void  SetChannel_SFX_Volume(const float& newVolume);
		float GetChannel_SFX_Volume();
		void  SetChannel_MUSIC_Volume(const float& newVolume);
		float GetChannel_MUSIC_Volume();
		void  SetChannel_AMBIENT_Volume(const float& newVolume);
		float GetChannel_AMBIENT_Volume();
		void  SetChannel_DIALOGUE_Volume(const float& newVolume);
		float GetChannel_DIALOGUE_Volume();
		///////////////////////////////////////////////

		//Sound Driver Control
		void SetCurrentSoundDriver(int index);									//Use GetAllSoundDriverData() to pick the available SoundDriver.

		//Listener Controls
		///////////////////////////////////////////////
		void Set3DListenerAttributes(const int& id, const vec3& position, const vec3& velocity, const vec3& forwardVec, const vec3& upVec); //forwardVec and upVec has to be unit vectors!
		///////////////////////////////////////////////

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
		friend class AudioSource;
		friend class AudioClipLoader;


		FMOD::System*		_Core_System				{ nullptr };		//Is updated on init, destroyed and nulled on shutdown.
		FMOD_RESULT			_result						{ FMOD_OK };		//Most recent result by the most recent FMOD function call.
		bool				_system_paused				{ false };			//Sets audiosystem to update or not
		int					_number_of_drivers			{ 0 };		//Updated on init. Describes the number of available sound driver that can play audio.
		int					_current_driver				{ 0 };		//Updated on init. Describes the current running sound driver.
		time_point			_time_it_was_initialized	{};			//Updated on Init()
		unsigned            _destroy_slot				{};
		const int			_max_channels				{ 512 };
		vector<AUDIOSYSTEM_DRIVERDATA> _driver_details	{};			//Describes each driver.


		//Useable after calling Init().
		FMOD::ChannelGroup* _channelGroup_MASTER		{ nullptr };	//All sounds are routed to MASTER.
		FMOD::ChannelGroup*	_channelGroup_SFX			{ nullptr };	//Sound Effects
		FMOD::ChannelGroup*	_channelGroup_MUSIC			{ nullptr };	//Music, by default is looped.
		FMOD::ChannelGroup*	_channelGroup_AMBIENT		{ nullptr };	//Ambient Sounds. This is similar to SFX. It is also OPTIONAL.
		FMOD::ChannelGroup*	_channelGroup_DIALOGUE		{ nullptr };	//Dialogue/Voice Overs. This is OPTIONAL.

		void ParseFMOD_RESULT(FMOD_RESULT);			//All fmod function returns an FMOD_RESULT. This function parses the result. Throws EXCEPTION_AudioSystem if a function fails.
		void ParseFMOD_RESULT_2(FMOD_RESULT);		//This does not update _result

	};


	//Exception Handler
	struct EXCEPTION_AudioSystem {
		string exceptionDetails {};
	};

	//Miscellaneous Data
	//These floats range from 0 to 100 (percent)
	struct AUDIOSYSTEM_CPUDATA {

		float dsp		{};	//DSP Mixing engine CPU usage
		float stream	{};	//Streaming engine CPU usage
		float geometry	{};	//Geometry engine CPU usage
		float update	{};	//System::update CPU usage
		float total		{};	//Total CPU usage
	};

	struct AUDIOSYSTEM_DRIVERDATA {

		int driverIndex					{ -1 };
		char driverName[512]			{};
		FMOD_GUID fmodID				{};
		int systemRate					{};
		FMOD_SPEAKERMODE speakerMode	{}; //Use FMOD_SPEAKERMODE
		int speakerModeChannels			{};
	};

}