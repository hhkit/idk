//////////////////////////////////////////////////////////////////////////////////
//@file		AudioClipLoader.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	A factory class that creates AudioClip resources.
/*
	PathHandle.CanOpen() first before getting full path.

*/
//////////////////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include <audio/AudioClipLoader.h>
#include <idk.h>
#include <audio/AudioSystem.h> //AudioSystem
//Dependency includes
#include <FMOD/core/fmod.hpp> //FMOD Core
#include <FMOD/core/fmod_errors.h> //ErrorString


namespace idk {
	ResourceBundle AudioClipLoader::LoadFile(PathHandle filePath)
	{
		auto newSound = Core::GetResourceManager().LoaderEmplaceResource<AudioClip>(); //Uses standard new alloc. Might need to change.
		auto& audioSystem = Core::GetSystem<AudioSystem>();
		auto* CoreSystem = audioSystem._Core_System;

		try
		{
			audioSystem.ParseFMOD_RESULT(CoreSystem->createSound(filePath.GetFullPath().data(), newSound->ConvertSettingToFMOD_MODE(), NULL, &(newSound->_soundHandle)));		//
			newSound->ReassignSoundGroup(SubSoundGroup::SubSoundGroup_SFX);
			newSound->UpdateMinMaxDistance();
		}
		catch (EXCEPTION_AudioSystem)
		{
			return ResourceBundle{};
		}

		//Retrieving Data Info for storage. This is a wrapper to store to the AudioClip for miscellaneous access.
		newSound->soundInfo.filePath = filePath.GetFullPath();
		char name[512];
		audioSystem.ParseFMOD_RESULT(newSound->_soundHandle->getName(name, 512));

		newSound->soundInfo.name = name;

		audioSystem.ParseFMOD_RESULT(newSound->_soundHandle->getFormat(&newSound->soundInfo.type, &newSound->soundInfo.format, &newSound->soundInfo.channels, &newSound->soundInfo.bits));

		newSound->_dirtymeta = true;
		//Push to list for management.
		//SoundList.push_back(newSound);

		return newSound;
	}
	ResourceBundle AudioClipLoader::LoadFile(PathHandle filePath, const MetaBundle& metabundle)
	{
		assert(false);
		return {};
	}
}