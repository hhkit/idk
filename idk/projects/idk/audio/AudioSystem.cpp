//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSystem.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	
//////////////////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Setting Up FMOD files:
	By default we will be using 64bit version of FMOD.
	Place FMOD\core\inc files and FMOD\studio\inc files into your include folder.
	Place FMOD\core\lib files and FMOD\studio\lib files into your library folder. This includes the .dll files. We will copy the .dll files during the post build event.

	Add: Project Properties -> Linker -> Input -> Additional Dependencies:			fmodstudioL_vc.lib; fmodL_vc.lib; (For Debug) or fmodstudio_vc.lib; fmod_vc.lib; (For Release)
	Add: Project Properties -> C++ -> General -> Additional Include Directories:	All FMODcore and FMODstudio files in a directory.

	Paste this code into Project Properties -> Build Events -> Post Build Event -> Command Line:
	FOR DEBUG:
		Xcopy $(ProjectDir)lib\fmodL.dll $(OutputPath) /i /f /y
		Xcopy $(ProjectDir)lib\fmodstudioL.dll $(OutputPath) /i /f /y
	FOR RELEASE:
		Xcopy $(ProjectDir)lib\fmod.dll $(OutputPath) /i /f /y
		Xcopy $(ProjectDir)lib\fmodstudio.dll $(OutputPath) /i /f /y

	You can write this in a .bat file if you want. But this copies the .dll files from your library folder to your build folder, so you dont have to manually copy them.
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/*
------------------------------ENGINE--------------------------------------
PROGRAMMER
Version Number dictates:


Manages audioEngine initializations
Handles assortments of audios, channel groups, and master
Handles serializations of own data,
It saves project configurations into its own file.

SERIALIZATIONS:
--------------Project-----------------
	Version Number of AudioEngine
	Number of Custom Channel Group

--------------Map-----------------



--------------GameObject-----------------
	Array of audio paths.
	Volume


FUNCTIONS:
--------------System Engine Level-----------------
	Initialize()

	Update()

	Shutdown()

	Log()

	GetNumberOfSoundsLoaded()
	GetInfoOfSoundsLoaded()

	ShowDebug()


--------------Map Level-----------------
	Load() //Loads all necessary audio in level

	Unload() //Unloads all audio in level.

--------------Game Object Level-----------------
	Play(int index)
	Play(string audioName)
	Play() //Current sound
	Stop()
	Pause()
	SetVolume(float i)
	GetVolume()
	SetPitch(float i)
	GetPitch()


PROJECT CONFIG:
	3D Settings - Doppler Scale
				- Distance Factor (How many units equate to 1 meters?)
				- Rolloff Scale



DEBUGGER:
	Can display current action.
	Can show amount of memory in use
	Can show what audio is loaded
	Log current profile



	//For Debug Compilation only
	//FMOD_Debug_Initialize(FMOD_DEBUG_LEVEL_LOG  | FMOD_DEBUG_DISPLAY_THREAD, FMOD_DEBUG_MODE_TTY,NULL,nullptr);



DESIGNER
-ChannelGroups-
Handles groups of audio, such as Music, or SFX or Voice. Can be created multiple, but default will have Music and SFX

------------------------------Audio--------------------------------------
Variables:
	Volume

*/

#pragma once
#include "stdafx.h" //Needed for every CPP. Precompiler
#include <audio/AudioSystem.h> //AudioSystem
#include <audio/AudioClip.h> //AudioClip
#include <audio/AudioClipLoader.h> //AudioClipLoader
#include <audio/AudioSource.h> //AudioClipFactory
#include <audio/AudioListener.h> //AudioClipFactory

#include <res/EasyFactory.h>

//Dependency includes
#include <FMOD/core/fmod.hpp> //FMOD Core
#include <FMOD/core/fmod_errors.h> //ErrorString

#include <sstream> //ostringstream
#include <res/ResourceHandle.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <core/GameState.inl>
#include <ds/span.inl>

namespace idk
{

	AudioSystem::AudioSystem() {}

	AudioSystem::~AudioSystem()
	{
	}

	void AudioSystem::Init()
	{
		Core::GetResourceManager().RegisterFactory<EasyFactory<AudioClip>>();
		Core::GetResourceManager().RegisterLoader<AudioClipLoader>(".wav");
		Core::GetResourceManager().RegisterLoader<AudioClipLoader>(".ogg");
		Core::GetResourceManager().RegisterLoader<AudioClipLoader>(".mp3");

		_destroy_slot = Core::GetGameState().OnObjectDestroy<AudioSource>() += [&](Handle<AudioSource> dying_source) //Add a call back to stop all its sounds before changing scene
		{
			dying_source->StopAll();
		};

		// Create the FMOD Core System object.
		ParseFMOD_RESULT(FMOD::System_Create(&_Core_System));

		// Initializes FMOD Core
		ParseFMOD_RESULT(_Core_System->init(_max_channels, FMOD_INIT_NORMAL, 0)); //1024 = number of channels that can be played on
		
		//Channel Group Setup
		ParseFMOD_RESULT(_Core_System->createChannelGroup("soundGroup_SFX",			&_channelGroup_SFX		));
		ParseFMOD_RESULT(_Core_System->createChannelGroup("soundGroup_MUSIC",		&_channelGroup_MUSIC	));
		ParseFMOD_RESULT(_Core_System->createChannelGroup("soundGroup_AMBIENT",		&_channelGroup_AMBIENT	));
		ParseFMOD_RESULT(_Core_System->createChannelGroup("soundGroup_DIALOGUE",	&_channelGroup_DIALOGUE	));
		ParseFMOD_RESULT(_Core_System->getMasterChannelGroup(&_channelGroup_MASTER));

		_channelGroup_MASTER		->setVolumeRamp(false);
		_channelGroup_SFX			->setVolumeRamp(false);
		_channelGroup_MUSIC			->setVolumeRamp(false);
		_channelGroup_AMBIENT		->setVolumeRamp(false);
		_channelGroup_DIALOGUE		->setVolumeRamp(false);
		//Get Number of Drivers available
		ParseFMOD_RESULT(_Core_System->getNumDrivers(&_number_of_drivers));

		//Driver info setup
		_driver_details.clear(); //In the event that audio engine is reinitialized
		if (_number_of_drivers != 0) {
			//std::cout << "Number of Drivers found: " << numberOfDrivers << std::endl;
			
			for (int i = 0; i < _number_of_drivers; ++i) {
				_driver_details.push_back(AUDIOSYSTEM_DRIVERDATA{});

				_driver_details[i].driverIndex = i;
				////Retrieve data
				//driverDetails[i].driverName[512];
				//driverDetails[i].fmodID;
				//driverDetails[i].systemRate;
				//driverDetails[i].speakerMode;
				//driverDetails[i].speakerModeChannels;

				//Get driver info
				ParseFMOD_RESULT(_Core_System->getDriverInfo(i, _driver_details[i].driverName, 512, &_driver_details[i].fmodID, &_driver_details[i].systemRate, &_driver_details[i].speakerMode, &_driver_details[i].speakerModeChannels));

				//DRIVER NAME
				//std::cout << "Sound Driver (" << i << "): \t";
				//std::cout << driverName << std::endl;
				//GUID
				//std::cout << "    GUID: \t\t" << std::hex;
				//std::cout << fmodID.Data1 << "-" << fmodID.Data2 << "-" << fmodID.Data3 << "-";
				//for (int j = 0; j < 8; ++j) {
				//	std::cout << static_cast<int>(fmodID.Data4[j]);
				//}
				//std::cout << std::dec << std::endl;

				//SPEAKER RATE
				//std::cout << "    Sample Rate: \t" << systemRate << " kHz" << std::endl;
				//NUMBER OF CHANNELS
				//std::cout << "    Number of Channels: " << speakerModeChannels << std::endl;


			}
		}
		else {
			//If no sound driver, set output to no sound
			ParseFMOD_RESULT(_Core_System->setOutput(FMOD_OUTPUTTYPE_NOSOUND));
			//std::cout << "No sound drivers found! Audio will set to NOSOUND." << std::endl;
			_current_driver = -1;
		}


		ParseFMOD_RESULT(_Core_System->getDriver(&_current_driver));
		//std::cout << "Current driver index in use:" << currentDriver << std::endl;

		_time_it_was_initialized = time_point::clock::now();

	}

	void AudioSystem::Update(span<AudioSource> audio_sources, span<AudioListener> audio_listeners)
	{


		//Update all the audio source here too!
		for (auto& elem : audio_sources)
		{
			elem.UpdateAudioClips();
		}

		//Check which listeners are active. At best there will always be at most, 4 listeners in the game. And it is not going to be constantly toggable.
		//Best case O(1), Worst case O(2N)
		int num_active_listeners = 0;
		for (auto& elem : audio_listeners)
		{
			if (elem.is_active)
				++num_active_listeners;
		}

		if (num_active_listeners) {
			_Core_System->set3DNumListeners(num_active_listeners);
			int listener_index = 0;
			for (auto& elem : audio_listeners)
			{
				if (elem.is_active) {
					elem.UpdateListenerPosition(listener_index);
					++listener_index;
				}
			}
		}
		else {
			_Core_System->set3DNumListeners(1); //Default to 1.
			//Set3DListenerAttributes(0,vec3{}, vec3{}, vec3{}, vec3{}); //If there is no active listener, dont need to update the default listener position!
		}



		// Get Updates the core system by a tick
		ParseFMOD_RESULT(_Core_System->update());
	}

	void AudioSystem::UpdateTestCaseOnly()
	{

		// Get Updates the core system by a tick
		ParseFMOD_RESULT(_Core_System->update());

	}
	void AudioSystem::StopAllAudio()
	{
		int numOfChannelsPlaying = 0; //A gate to reduce calls
		ParseFMOD_RESULT(_Core_System->getChannelsPlaying(&numOfChannelsPlaying));


		for (int i = 0; i < _max_channels && numOfChannelsPlaying; ++i) {
			FMOD::Channel* channelPtr;
			_result = _Core_System->getChannel(i, &channelPtr);

			bool isPlaying = false;
			_result = channelPtr->isPlaying(&isPlaying); //This channel pointer can never be null after calling getChannel! It can be invalid, so calling isPlaying can result in invalid handle, but its ok

			if (_result == FMOD_OK && isPlaying) {
				ParseFMOD_RESULT(channelPtr->stop());
			}
		}
	}
	void AudioSystem::SetSystemPaused(bool is_system_paused)
	{
		_system_paused = is_system_paused;

		int numOfChannelsPlaying = 0; //A gate to reduce calls
		ParseFMOD_RESULT(_Core_System->getChannelsPlaying(&numOfChannelsPlaying));

		for (int i = 0; i < _max_channels && numOfChannelsPlaying; ++i) {
			FMOD::Channel* channelPtr = nullptr;
			_result = _Core_System->getChannel(i, &channelPtr); //getChannel does not point to a nullptr, rather an allocated empty space in FMOD

			bool isPlaying = false;
			_result = channelPtr->isPlaying(&isPlaying); //This channel pointer can never be null after calling getChannel! It can be invalid, so calling isPlaying can result in invalid handle, but its ok

			if (_result == FMOD_OK && isPlaying) {
				ParseFMOD_RESULT(channelPtr->setPaused(_system_paused));
				--numOfChannelsPlaying;
			}
		}


	}

	void AudioSystem::Set3DListenerAttributes(const int& id, const vec3& pos, const vec3&vel,const vec3& forwardVec, const vec3& upVec)
	{
		//Zero denotes the listener id. Since there is only one listener, this is always zero.
		FMOD_VECTOR fmodPos			{ pos.x,pos.y,pos.z };
		FMOD_VECTOR fmodVel			{ vel.x,vel.y,vel.z };
		FMOD_VECTOR fmodForwardVec	{ forwardVec.x,forwardVec.y,forwardVec.z };
		FMOD_VECTOR fmodUpVec		{ upVec.x,upVec.y,upVec.z };

		//NOTES: Forward is axisZ+, Up is axisY+
		ParseFMOD_RESULT(_Core_System->set3DListenerAttributes(id, &fmodPos, &fmodVel, &fmodForwardVec, &fmodUpVec));
	}

	void AudioSystem::Shutdown()
	{
		Core::GetGameState().OnObjectDestroy<AudioSource>() -= _destroy_slot;

		StopAllAudio();
			
		//Closes sound groups. Dont really have to do this, but this is for cleanliness.
		ParseFMOD_RESULT(_channelGroup_MUSIC	->release()); 
		ParseFMOD_RESULT(_channelGroup_SFX		->release()); 
		ParseFMOD_RESULT(_channelGroup_AMBIENT  ->release()); 
		ParseFMOD_RESULT(_channelGroup_DIALOGUE ->release());
		_channelGroup_MUSIC		= nullptr;
		_channelGroup_SFX		= nullptr;
		_channelGroup_AMBIENT	= nullptr;
		_channelGroup_DIALOGUE	= nullptr;
		_channelGroup_MASTER	= nullptr;

		//System close
		ParseFMOD_RESULT(_Core_System->release());

		// Cleanup
		_Core_System = nullptr;
	}

	void AudioSystem::SetMemoryAllocators(FMOD_MEMORY_ALLOC_CALLBACK useralloc, FMOD_MEMORY_REALLOC_CALLBACK userrealloc, FMOD_MEMORY_FREE_CALLBACK userfree)
	{
		FMOD::Memory_Initialize(NULL, 0, useralloc, userrealloc, userfree);
	}

	void AudioSystem::ParseFMOD_RESULT(FMOD_RESULT e)
	{
		_result = e;
		ParseFMOD_RESULT_2(_result);
	}


	void AudioSystem::ParseFMOD_RESULT_2(FMOD_RESULT e)
	{
		if (e != FMOD_OK)
			LOG_WARNING_TO(LogPool::AUDIO, "FMOD error! (%s)", FMOD_ErrorString(_result));
	}


	void AudioSystem::SetChannel_MASTER_Volume(const float& newVolume)
	{

		ParseFMOD_RESULT(_channelGroup_MASTER->setVolume(newVolume));
	}

	float AudioSystem::GetChannel_MASTER_Volume()
	{
		float returnVal = 0;
		ParseFMOD_RESULT(_channelGroup_MASTER->getVolume(&returnVal));
		return returnVal;
	}

	void AudioSystem::SetChannel_SFX_Volume(const float& newVolume)
	{
		ParseFMOD_RESULT(_channelGroup_SFX->setVolume(newVolume));
	}

	float AudioSystem::GetChannel_SFX_Volume()
	{
		float returnVal = 0;
		ParseFMOD_RESULT(_channelGroup_SFX->getVolume(&returnVal));
		return returnVal;
	}

	void AudioSystem::SetChannel_MUSIC_Volume(const float& newVolume)
	{
		ParseFMOD_RESULT(_channelGroup_MUSIC->setVolume(newVolume));
	}

	float AudioSystem::GetChannel_MUSIC_Volume()
	{
		float returnVal = 0;
		ParseFMOD_RESULT(_channelGroup_MUSIC->getVolume(&returnVal));
		return returnVal;
	}

	void AudioSystem::SetChannel_AMBIENT_Volume(const float& newVolume)
	{
		ParseFMOD_RESULT(_channelGroup_AMBIENT->setVolume(newVolume));
	}

	float AudioSystem::GetChannel_AMBIENT_Volume()
	{
		float returnVal = 0;
		ParseFMOD_RESULT(_channelGroup_AMBIENT->getVolume(&returnVal));
		return returnVal;
	}

	void AudioSystem::SetChannel_DIALOGUE_Volume(const float& newVolume)
	{
		ParseFMOD_RESULT(_channelGroup_DIALOGUE->setVolume(newVolume));
	}

	float AudioSystem::GetChannel_DIALOGUE_Volume()
	{
		float returnVal = 0;
		ParseFMOD_RESULT(_channelGroup_DIALOGUE->getVolume(&returnVal));
		return returnVal;
	}

	void AudioSystem::SetCurrentSoundDriver(int index)
	{
		try {
			ParseFMOD_RESULT(_Core_System->setDriver(index));
			ParseFMOD_RESULT(_Core_System->getDriver(&_current_driver));

		}
		catch (EXCEPTION_AudioSystem i) { //Catch out of bounds index.
			//Do nothing if out of bounds
			return;
		}
	}

	float AudioSystem::GetCPUPercentUsage()
	{
		return GetDetailedCPUPercentUsage().total;
	}

	AUDIOSYSTEM_CPUDATA AudioSystem::GetDetailedCPUPercentUsage() 
	{
		AUDIOSYSTEM_CPUDATA i{};
		ParseFMOD_RESULT(_Core_System->getCPUUsage(&i.dsp, &i.stream, &i.geometry, &i.update, &i.total));
		return i;
	}

	vector<AUDIOSYSTEM_DRIVERDATA> AudioSystem::GetAllSoundDriverData() const
	{
		return _driver_details;
	}

	int AudioSystem::GetCurrentSoundDriverIndex() const
	{
		return _current_driver;
	}

	time_point AudioSystem::GetTimeInitialized() const
	{
		return _time_it_was_initialized;
	}

	void AudioSystem::GetMemoryStats(int* currentBytesAllocated, int* maxBytesAllocated, bool precise)
	{
		ParseFMOD_RESULT(FMOD_Memory_GetStats(currentBytesAllocated, maxBytesAllocated, precise));

		//std::cout << "Current Allocated Memory: " << *currentBytesAllocated << " bytes" << std::endl;
		//std::cout << "Maximum Allocated Memory: " << *maxBytesAllocated << " bytes" << std::endl;
	}

	const char* AudioSystem::FMOD_SOUND_TYPE_TO_C_STR(FMOD_SOUND_TYPE i)
	{
		switch (i) {
		default:
			return "FMOD_SOUND_TYPE Error! Enum not valid!";
		case FMOD_SOUND_TYPE_UNKNOWN:
			return "FMOD_SOUND_TYPE_UNKNOWN";
		case FMOD_SOUND_TYPE_AIFF:
			return "FMOD_SOUND_TYPE_AIFF";
		case FMOD_SOUND_TYPE_ASF:
			return "FMOD_SOUND_TYPE_ASF";
		case FMOD_SOUND_TYPE_DLS:
			return "FMOD_SOUND_TYPE_DLS";
		case FMOD_SOUND_TYPE_FLAC:
			return "FMOD_SOUND_TYPE_FLAC";
		case FMOD_SOUND_TYPE_FSB:
			return "FMOD_SOUND_TYPE_FSB";
		case FMOD_SOUND_TYPE_IT:
			return "FMOD_SOUND_TYPE_IT";
		case FMOD_SOUND_TYPE_MIDI:
			return "FMOD_SOUND_TYPE_MIDI";
		case FMOD_SOUND_TYPE_MOD:
			return "FMOD_SOUND_TYPE_MOD";
		case FMOD_SOUND_TYPE_MPEG:
			return "FMOD_SOUND_TYPE_MPEG";
		case FMOD_SOUND_TYPE_OGGVORBIS:
			return "FMOD_SOUND_TYPE_OGGVORBIS";
		case FMOD_SOUND_TYPE_PLAYLIST:
			return "FMOD_SOUND_TYPE_PLAYLIST";
		case FMOD_SOUND_TYPE_RAW:
			return "FMOD_SOUND_TYPE_RAW";
		case FMOD_SOUND_TYPE_S3M:
			return "FMOD_SOUND_TYPE_S3M";
		case FMOD_SOUND_TYPE_USER:
			return "FMOD_SOUND_TYPE_USER";
		case FMOD_SOUND_TYPE_WAV:
			return "FMOD_SOUND_TYPE_WAV";
		case FMOD_SOUND_TYPE_XM:
			return "FMOD_SOUND_TYPE_XM";
		case FMOD_SOUND_TYPE_XMA:
			return "FMOD_SOUND_TYPE_XMA";
		case FMOD_SOUND_TYPE_AUDIOQUEUE:
			return "FMOD_SOUND_TYPE_AUDIOQUEUE";
		case FMOD_SOUND_TYPE_AT9:
			return "FMOD_SOUND_TYPE_AT9";
		case FMOD_SOUND_TYPE_VORBIS:
			return "FMOD_SOUND_TYPE_VORBIS";
		case FMOD_SOUND_TYPE_MEDIA_FOUNDATION:
			return "FMOD_SOUND_TYPE_MEDIA_FOUNDATION";
		case FMOD_SOUND_TYPE_MEDIACODEC:
			return "FMOD_SOUND_TYPE_MEDIACODEC";
		case FMOD_SOUND_TYPE_FADPCM:
			return "FMOD_SOUND_TYPE_FADPCM";
		case FMOD_SOUND_TYPE_MAX:
			return "FMOD_SOUND_TYPE_MAX";
		case FMOD_SOUND_TYPE_FORCEINT:
			return "FMOD_SOUND_TYPE_FORCEINT";
		}

	}

	const char* AudioSystem::FMOD_SOUND_FORMAT_TO_C_STR(FMOD_SOUND_FORMAT i)
	{
		switch (i) {
		default:
			return "FMOD_SOUND_FORMAT Error! Enum not valid!";
		case FMOD_SOUND_FORMAT_NONE:
			return "FMOD_SOUND_FORMAT_NONE";
		case FMOD_SOUND_FORMAT_PCM8:
			return "FMOD_SOUND_FORMAT_PCM8";
		case FMOD_SOUND_FORMAT_PCM16:
			return "FMOD_SOUND_FORMAT_PCM16";
		case FMOD_SOUND_FORMAT_PCM24:
			return "FMOD_SOUND_FORMAT_PCM24";
		case FMOD_SOUND_FORMAT_PCM32:
			return	"FMOD_SOUND_FORMAT_PCM32";
		case FMOD_SOUND_FORMAT_PCMFLOAT:
			return	"FMOD_SOUND_FORMAT_PCMFLOAT";
		case FMOD_SOUND_FORMAT_BITSTREAM:
			return	"FMOD_SOUND_FORMAT_BITSTREAM";
		case FMOD_SOUND_FORMAT_MAX:
			return	"FMOD_SOUND_FORMAT_MAX";
		case FMOD_SOUND_FORMAT_FORCEINT:
			return	"FMOD_SOUND_FORMAT_FORCEINT";

		}

	}

	const char* AudioSystem::FMOD_SPEAKERMODE_TO_C_STR(FMOD_SPEAKERMODE i)
	{
		switch (i) {
		default:
			return "FMOD_SPEAKERMODE Error! Enum not valid!";
		case FMOD_SPEAKERMODE_DEFAULT:
			return "FMOD_SPEAKERMODE_DEFAULT";
		case FMOD_SPEAKERMODE_RAW:
			return "FMOD_SPEAKERMODE_RAW";
		case FMOD_SPEAKERMODE_MONO:
			return "FMOD_SPEAKERMODE_MONO";
		case FMOD_SPEAKERMODE_STEREO:
			return "FMOD_SPEAKERMODE_STEREO";
		case FMOD_SPEAKERMODE_QUAD:
			return "FMOD_SPEAKERMODE_QUAD";
		case FMOD_SPEAKERMODE_SURROUND:
			return "FMOD_SPEAKERMODE_SURROUND";
		case FMOD_SPEAKERMODE_5POINT1:
			return "FMOD_SPEAKERMODE_5POINT1";
		case FMOD_SPEAKERMODE_7POINT1:
			return "FMOD_SPEAKERMODE_7POINT1";
		case FMOD_SPEAKERMODE_7POINT1POINT4:
			return "FMOD_SPEAKERMODE_7POINT1POINT4";
		case FMOD_SPEAKERMODE_MAX:
			return "FMOD_SPEAKERMODE_MAX";
		}

	}

}