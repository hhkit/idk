

#include <stdafx.h>
#include "AudioClipFactory.h"
#include <idk.h>
#include <audio/AudioSystem.h> //AudioSystem
//Dependency includes
#include <FMOD/core/fmod.hpp> //FMOD Core
#include <FMOD/core/fmod_errors.h> //ErrorString


namespace idk {
	unique_ptr<AudioClip> idk::AudioClipFactory::Create()
	{
		return unique_ptr<AudioClip>();
	}

	unique_ptr<AudioClip> AudioClipFactory::Create(string_view filePath)
	{
		auto newSound = std::make_unique<AudioClip>(); //Uses standard new alloc. Might need to change.
		auto& audioSystem = Core::GetSystem<AudioSystem>();
		auto* CoreSystem = audioSystem._Core_System;

		try {
			audioSystem.ParseFMOD_RESULT(CoreSystem->createSound(filePath.data(), FMOD_LOOP_OFF | FMOD_3D, NULL, &(newSound->_soundHandle)));		//
			newSound->is3Dsound = true;
			newSound->loop = false;
			newSound->_soundHandle->setSoundGroup(audioSystem._soundGroup_SFX);

		}
		catch (EXCEPTION_AudioSystem i) { //If an error occurs here, delete newSound and return nullptr
			//std::cout << i.exceptionDetails << "Returning nullptr.\n";
			return nullptr;
		}
		//Retrieving Data Info for storage. This is a wrapper to store to the AudioClip for miscellaneous access.
		newSound->soundInfo.filePath = filePath;
		char name[512];
		audioSystem.ParseFMOD_RESULT(newSound->_soundHandle->getName(name, 512));
		newSound->soundInfo.name = name;
		audioSystem.ParseFMOD_RESULT(newSound->_soundHandle->getFormat(&newSound->soundInfo.type, &newSound->soundInfo.format, &newSound->soundInfo.channels, &newSound->soundInfo.bits));
		
		//Push to list for management.
		//SoundList.push_back(newSound);
		
		return newSound;
	}

	unique_ptr<AudioClip> idk::AudioClipFactory::Create(string_view filePath, const ResourceMeta&)
	{
		return Create(filePath);
	}
}