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

#include "audio/AudioSystem.h" //AudioSystem
#include "audio/AudioClip.h" //AudioClip

//Dependency includes
#include "FMOD/core/fmod.hpp" //FMOD Core
#include "FMOD/core/fmod_errors.h" //ErrorString

#include <iostream> //cout
#include <iomanip> //put_time
//#include <conio.h> //getch todo
#include <ctime> //
#include <sstream> //ostringstream
#include <filesystem> //filesystem


namespace idk
{
	AudioSystem::AudioSystem()
		: CoreSystem{ nullptr }
		, result{ FMOD_OK }
		, timeItWasInitialized{}
		, numberOfDrivers{ 0 }
	{
	}

	AudioSystem::~AudioSystem()
	{
	}

	void AudioSystem::Init()
	{
	}

	void AudioSystem::Shutdown()
	{
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

}