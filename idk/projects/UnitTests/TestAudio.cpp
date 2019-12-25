//////////////////////////////////////////////////////////////////////////////////
//@file		TestAudio.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	Unit test for audio system. Uses FMOD.
//////////////////////////////////////////////////////////////////////////////////


#include "pch.h" // gtest.h
#include <reflect/reflect.h>
#include "FMOD/core/fmod.hpp" //FMOD Core
#include "FMOD/core/fmod_errors.h" //ErrorString
#include <scene/Scene.inl>
#include <core/GameObject.inl>	
#include <audio/AudioSystem.h>	
#include <audio/AudioClip.h>	
#include <audio/AudioListener.h>
#include <file/FileSystem.h>
#include <common/Transform.h>	
#include <core/Core.inl>
#include "TestApplication.h"

TEST(Audio, AudioSystemClassTest)
{
	using namespace idk;
	Core c;
    c.AddSystem<TestApplication>();
	auto& test = c.GetSystem<AudioSystem>();
    EXPECT_NO_THROW(c.Setup());

    c.GetSystem<FileSystem>().Mount(TEST_DATA_PATH, "/assets");

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

	std::cout << "Searching for \\audio\\My Delirium - Ladyhawke (Lyrics).mp3 in directory...\n";

	RscHandle<AudioClip> audioPtr1 = *Core::GetResourceManager().Load<AudioClip>("/assets/audio/My Delirium - Ladyhawke (Lyrics).mp3");

	if (!audioPtr1) { //Check if null is given
		std::cout << "Audio path not found, skipping test...\n";

		try {
			c.Shutdown();
		}
		catch (EXCEPTION_AudioSystem i) {
			std::cout << "//////////////////////////////////////\n";

			std::cout << i.exceptionDetails << std::endl;
			EXPECT_TRUE(false);
		}

		return;

	}
	else {
		std::cout << "Found file!\n";

	}


    RscHandle<AudioClip> audioPtr2 = audioPtr1;
	RscHandle<AudioClip> audioPtr3 = *Core::GetResourceManager().Load<AudioClip>("/assets/audio/25secClosing_IZHA.wav");

	std::cout << "Playing first sound in default SFX group...\n";


	//audioPtr1->Play();
	time_point timeStartTest = Clock::now();
	seconds elapsed = time_point::clock::now() - timeStartTest;

	bool testCase1 = false;
	bool testCase2 = false;
	bool testCase3 = false;
	bool testCase4 = false;
	bool testCase5 = false;
	bool testCase6 = false;



	while (elapsed.count() < 10) { //
		try {
			test.UpdateTestCaseOnly();
			elapsed = time_point::clock::now() - timeStartTest;
			


			//if (elapsed.count() > 2 && !testCase1) {
			//	std::cout << "Setting SFX group to volume 0.5\n";
			//	test.SetChannel_SFX_Volume(0.5f);
			//	testCase1 = true;
			//}
			//if (elapsed.count() > 3 && !testCase2) {
			//	std::cout << "Setting Master group to volume 0.5\n";
			//	test.SetChannel_MASTER_Volume(0.5f);
			//	testCase2 = true;
			//}

			if (elapsed.count() > 4 && !testCase3) {
				std::cout << "Playing second sound to MUSIC group\n";
				audioPtr2->ReassignSoundGroup(SubSoundGroup::SubSoundGroup_MUSIC);

				//audioPtr2->Play();
				testCase3 = true;
			}
			if (elapsed.count() > 5 && !testCase4) {
				std::cout << "Stopping first music\n";

				//audioPtr1->Stop();
				testCase4 = true;
			}

			if (elapsed.count() > 6 && !testCase5) {
				std::cout << "Playing another misc music and stopping second music\n";

				//audioPtr3->Play();
				//audioPtr2->Stop();

				testCase5 = true;
			}
			if (elapsed.count() > 8 && !testCase6) {
				std::cout << "Setting third music to loop and high pitched\n";
				//audioPtr3->Play(); //Plays another audio

				testCase6 = true;
			}

		}
		catch (EXCEPTION_AudioSystem i) {
			std::cout << i.exceptionDetails << std::endl;
			EXPECT_TRUE(false);
			break;
		}
	}
	try {
		c.Shutdown();
	}
	catch (EXCEPTION_AudioSystem i) {
		std::cout << i.exceptionDetails << std::endl;
		EXPECT_TRUE(false);
	}
	// Cleanup
}
