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

TEST(Audio, AudioBasicSystemInitializations)
{
	FMOD_RESULT result; //Most recent result by the most recent FMOD function call.
	FMOD::System* CoreSystem;

	// Create the FMOD Core System object.
	result = FMOD::System_Create(&CoreSystem);
	EXPECT_TRUE(result == FMOD_OK);

	// Initializes FMOD Core
	result = CoreSystem->init(512, FMOD_INIT_NORMAL, 0);
	EXPECT_TRUE(result == FMOD_OK);

	// Get Updates the core system by a tick
	result = CoreSystem->update();
	EXPECT_TRUE(result == FMOD_OK);

	result = CoreSystem->release(); //Closes and releases memory.
	EXPECT_TRUE(result == FMOD_OK);

	// Cleanup
	CoreSystem = nullptr;
}

