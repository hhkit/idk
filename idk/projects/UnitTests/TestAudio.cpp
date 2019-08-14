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
#include <iostream>	


TEST(Audio, AudioSystemClassTest)
{
	using namespace idk;
	AudioSystem test{};
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


	try {
		test.Run();
	}
	catch (EXCEPTION_AudioSystem i) {
		std::cout << i.exceptionDetails << std::endl;
		EXPECT_TRUE(false);
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

