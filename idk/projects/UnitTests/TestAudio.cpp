//////////////////////////////////////////////////////////////////////////////////
//@file		TestAudio.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	Unit test for audio system. Uses FMOD.
//////////////////////////////////////////////////////////////////////////////////


#include "pch.h" // gtest.h
#include "FMOD/core/fmod.hpp" //FMOD Core
#include "FMOD/core/fmod_errors.h" //ErrorString
#include <audio/AudioSystem.h>	
#include <audio/AudioClip.h>	
#include <core/Core.h>
#include <iostream>	
#include <filesystem> //Using this until our filesystem is up	


TEST(Audio, AudioSystemClassTest)
{
	using namespace idk;
	Core c;
	AudioSystem& test = Core::GetSystem<AudioSystem>();
	c.Setup();
	try { 
		test.Init(); 
	}
	catch (EXCEPTION_AudioSystem i) {
		std::cout << i.exceptionDetails << std::endl;
		EXPECT_TRUE(false);
	}
	std::cout << "Getting all available sound drivers... "<< std::endl;

	auto i = test.GetAllSoundDriverData();
	for(auto & j : i) {
		std::cout << "  INDEX: " << j.driverIndex << std::endl;
		std::cout << "  NAME: "<< j.driverName << std::endl;
		std::cout << "  ID: " << std::hex << j.fmodID.Data1 << "-"
			<< std::hex << j.fmodID.Data2 << "-"
			<< std::hex << j.fmodID.Data3 << "-";
		for (int k = 0; k < 8; ++k) {
			std::cout << static_cast<int>(j.fmodID.Data4[k]);
		}
		std::cout << std::dec << std::endl ;

		std::cout << "  SPEAKERMODE: " << test.FMOD_SPEAKERMODE_TO_C_STR(j.speakerMode) << std::endl;
		std::cout << "  SPEAKERMODECHANNELS: " << j.speakerModeChannels << std::endl;
		std::cout << "  SYSTEMRATE: " << j.systemRate << std::endl << std::endl;

	}

	
	std::cout << "Creating Test audio in SFX Channel:\n";
	std::string path = std::filesystem::current_path().string();
	std::cout << "Current Working Directory: " << path;
	std::cout << "\n";

	path.append("\\SampleSounds\\25secClosing_IZHA.wav"); 


	RscHandle<AudioClip> audioPtr = Core::GetResourceManager().Create<AudioClip>(path);

	if (!audioPtr) { //Check if null is given
		std::cout << "Audio path not found, skipping test...\n";

		try {
			test.Shutdown();
		}
		catch (EXCEPTION_AudioSystem i) {
			std::cout << i.exceptionDetails << std::endl;
			EXPECT_TRUE(false);
		}

		return;

	}
	audioPtr->Play();
	time_point timeStartTest = Clock::now();
	seconds elapsed = time_point::clock::now() - timeStartTest;

	bool testCase1 = false;
	bool testCase2 = false;





	while (elapsed.count() < 30) { //Once 3 seconds have passed, exit
		try {
			test.Update();
			elapsed = time_point::clock::now() - timeStartTest;
			if (elapsed.count() > 5 && !testCase1) {
				std::cout << "Setting SFX group to volume 0.5\n";
				test.SetChannel_SFX_Volume(0.5f);
				testCase1 = true;
			}
			if (elapsed.count() > 8 && !testCase2) {
				std::cout << "Setting Master group to volume 0.5\n";
				test.SetChannel_MASTER_Volume(0.5f);
				testCase2 = true;
			}
		}
		catch (EXCEPTION_AudioSystem i) {
			std::cout << i.exceptionDetails << std::endl;
			EXPECT_TRUE(false);
			break;
		}
	}
	try {
		test.Shutdown();
	}
	catch (EXCEPTION_AudioSystem i) {
		std::cout << i.exceptionDetails << std::endl;
		EXPECT_TRUE(false);
	}
	// Cleanup
}

