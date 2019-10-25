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
#include <res/MetaBundle.h>
//Dependency includes
#include <FMOD/core/fmod.hpp> //FMOD Core
#include <FMOD/core/fmod_errors.h> //ErrorString


namespace idk {
	ResourceBundle AudioClipLoader::LoadFile(PathHandle filePath, const MetaBundle& metabundle)
	{
		const auto meta = metabundle.FetchMeta<AudioClip>();

		const auto newSound = meta 
			? Core::GetResourceManager().LoaderEmplaceResource<AudioClip>(meta->guid)
			: Core::GetResourceManager().LoaderEmplaceResource<AudioClip>(); //Uses standard new alloc. Might need to change.
		auto& audioSystem = Core::GetSystem<AudioSystem>();
		auto* CoreSystem = audioSystem._Core_System;

		try
		{
			audioSystem.ParseFMOD_RESULT(CoreSystem->createSound(filePath.GetFullPath().data(), FMOD_DEFAULT, NULL, &(newSound->_soundHandle)));		//
			newSound->ReassignSoundGroup(SubSoundGroup::SubSoundGroup_SFX);
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

		//Push to list for management.
		//SoundList.push_back(newSound);

		return newSound;
	}
}