//////////////////////////////////////////////////////////////////////////////////
//@file		AudioSystem.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
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

//Dependency includes
#include <FMOD/core/fmod.hpp> //FMOD Core
#include <FMOD/core/fmod_errors.h> //ErrorString

#include <iostream> //cout
#include <iomanip> //put_time
#include <ctime> //
#include <sstream> //ostringstream
#include <filesystem> //filesystem

namespace idk
{
	AudioSystem::AudioSystem()
		: CoreSystem			{ nullptr }
		, result				{ FMOD_OK }
		, timeItWasInitialized	{}
		, numberOfDrivers		{ 0 }
		, currentDriver			{ 0 }
		, channelGroup_MASTER	{ nullptr }
		, channelGroup_MUSIC	{ nullptr }
		, channelGroup_SFX		{ nullptr }
		, channelGroup_AMBIENT	{ nullptr }
		, channelGroup_DIALOGUE	{ nullptr }
	{
	}

	AudioSystem::~AudioSystem()
	{
	}

	void AudioSystem::Init()
	{

		// Create the FMOD Core System object.
		ParseFMOD_RESULT(FMOD::System_Create(&CoreSystem));

		// Initializes FMOD Core
		ParseFMOD_RESULT(CoreSystem->init(512, FMOD_INIT_NORMAL, 0)); //512 = number of channels that can be played on
		
		//Channel Group Setup
		ParseFMOD_RESULT(CoreSystem->createChannelGroup("MUSIC", &channelGroup_MUSIC));
		ParseFMOD_RESULT(CoreSystem->createChannelGroup("SFX", &channelGroup_SFX));
		ParseFMOD_RESULT(CoreSystem->createChannelGroup("AMBIENT", &channelGroup_AMBIENT));
		ParseFMOD_RESULT(CoreSystem->createChannelGroup("DIALOGUE", &channelGroup_DIALOGUE));
		ParseFMOD_RESULT(CoreSystem->getMasterChannelGroup(&channelGroup_MASTER));

		//Get Number of Drivers available
		ParseFMOD_RESULT(CoreSystem->getNumDrivers(&numberOfDrivers));

		//Driver info setup
		driverDetails.clear(); //In the event that audio engine is reinitialized
		if (numberOfDrivers != 0) {
			//std::cout << "Number of Drivers found: " << numberOfDrivers << std::endl;
			
			for (int i = 0; i < numberOfDrivers; ++i) {
				driverDetails.push_back(AUDIOSYSTEM_DRIVERDATA{});

				driverDetails[i].driverIndex = i;
				////Retrieve data
				//driverDetails[i].driverName[512];
				//driverDetails[i].fmodID;
				//driverDetails[i].systemRate;
				//driverDetails[i].speakerMode;
				//driverDetails[i].speakerModeChannels;

				//Get driver info
				ParseFMOD_RESULT(CoreSystem->getDriverInfo(i, driverDetails[i].driverName, 512, &driverDetails[i].fmodID, &driverDetails[i].systemRate, &driverDetails[i].speakerMode, &driverDetails[i].speakerModeChannels));

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
			ParseFMOD_RESULT(CoreSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND));
			//std::cout << "No sound drivers found! Audio will set to NOSOUND." << std::endl;
			currentDriver = -1;
		}


		ParseFMOD_RESULT(CoreSystem->getDriver(&currentDriver));
		//std::cout << "Current driver index in use:" << currentDriver << std::endl;

		timeItWasInitialized = time_point::clock::now();

	}

	void AudioSystem::Run()
	{
		// Get Updates the core system by a tick
		ParseFMOD_RESULT(CoreSystem->update());
	}
	void AudioSystem::Shutdown()
	{
		//Closes and releases memory.
		ParseFMOD_RESULT(CoreSystem->release());

		// Cleanup
		CoreSystem = nullptr;
	}

	void AudioSystem::SetMemoryAllocators(FMOD_MEMORY_ALLOC_CALLBACK useralloc, FMOD_MEMORY_REALLOC_CALLBACK userrealloc, FMOD_MEMORY_FREE_CALLBACK userfree)
	{
		FMOD::Memory_Initialize(NULL, 0, useralloc, userrealloc, userfree);
	}

	void AudioSystem::ParseFMOD_RESULT(FMOD_RESULT e)
	{
		result = e;
		if (result != FMOD_OK)
		{
			std::ostringstream stringStream;
			stringStream << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl; //Puts string into stream
			EXCEPTION_AudioSystem exception;
			exception.exceptionDetails = stringStream.str();
			throw exception;
		}
	}

	float AudioSystem::GetCPUPercentUsage() 
	{
		return GetDetailedCPUPercentUsage().total;
	}

	AUDIOSYSTEM_CPUDATA AudioSystem::GetDetailedCPUPercentUsage() 
	{
		AUDIOSYSTEM_CPUDATA i{};
		ParseFMOD_RESULT(CoreSystem->getCPUUsage(&i.dsp, &i.stream, &i.geometry, &i.update, &i.total));
		return i;
	}

	vector<AUDIOSYSTEM_DRIVERDATA> AudioSystem::GetAllSoundDriverData() const
	{
		return driverDetails;
	}

	int AudioSystem::GetCurrentSoundDriverIndex() const
	{
		return currentDriver;
	}

	time_point AudioSystem::GetTimeInitialized() const
	{
		return timeItWasInitialized;
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