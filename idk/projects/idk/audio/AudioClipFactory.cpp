

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
		auto* CoreSystem = audioSystem.CoreSystem;

		AudioClip::SubSoundGroup sndGrp = AudioClip::SubSoundGroup_SFX;
		try {
			switch (sndGrp) {
			default:
			case AudioClip::SubSoundGroup_SFX:
				audioSystem.ParseFMOD_RESULT(CoreSystem->createSound(filePath.data(), FMOD_LOOP_OFF | FMOD_3D, NULL, &(newSound->soundHandle)));		//
				newSound->soundHandle->setSoundGroup(audioSystem.soundGroup_SFX);

				break;
			case AudioClip::SubSoundGroup_MUSIC:
				audioSystem.ParseFMOD_RESULT(CoreSystem->createSound(filePath.data(), FMOD_LOOP_NORMAL | FMOD_2D | FMOD_CREATESTREAM, NULL, &(newSound->soundHandle)));	//
				newSound->soundHandle->setSoundGroup(audioSystem.soundGroup_MUSIC);

				break;
			case AudioClip::SubSoundGroup_AMBIENT:
				audioSystem.ParseFMOD_RESULT(CoreSystem->createSound(filePath.data(), FMOD_LOOP_NORMAL | FMOD_3D, NULL, &(newSound->soundHandle)));	//
				newSound->soundHandle->setSoundGroup(audioSystem.soundGroup_AMBIENT);

				break;
			case AudioClip::SubSoundGroup_DIALOGUE:
				audioSystem.ParseFMOD_RESULT(CoreSystem->createSound(filePath.data(), FMOD_LOOP_OFF | FMOD_3D, NULL, &(newSound->soundHandle)));		//
				newSound->soundHandle->setSoundGroup(audioSystem.soundGroup_DIALOGUE);

				break;

			}
		}
		catch (EXCEPTION_AudioSystem i) { //If an error occurs here, delete newSound and return nullptr
			//std::cout << i.exceptionDetails << "Returning nullptr.\n";
			return nullptr;
		}
		//Retrieving Data Info for storage. This is a wrapper to store to the AudioClip for miscellaneous access.
		newSound->soundInfo.filePath = filePath;
		char name[512];
		audioSystem.ParseFMOD_RESULT(newSound->soundHandle->getName(name, 512));
		newSound->soundInfo.name = name;
		audioSystem.ParseFMOD_RESULT(newSound->soundHandle->getFormat(&newSound->soundInfo.type, &newSound->soundInfo.format, &newSound->soundInfo.channels, &newSound->soundInfo.bits));
		
		//Push to list for management.
		//SoundList.push_back(newSound);
		
		return newSound;
	}

	unique_ptr<AudioClip> idk::AudioClipFactory::Create(string_view filePath, const ResourceMeta&)
	{
		return Create(filePath);
	}
}