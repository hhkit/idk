//////////////////////////////////////////////////////////////////////////////////
//@file		AudioClipInfo.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		12 AUG 2019
//@brief	A data structure to hold only information of a sound.
//////////////////////////////////////////////////////////////////////////////////



#pragma once

#include "FMOD/core/fmod_common.h" //FMOD Enums. This is included in the header file only because this is the only thing that should be exposed.
#include <string> //string

namespace idk
{
	//A data structure to hold only information of a sound. This is updated when CreateSound is called. When released, this is reset.
	struct AudioClipInfo {
		AudioClipInfo()
			: filePath{}, name{}, type{ FMOD_SOUND_TYPE_UNKNOWN }, format{ FMOD_SOUND_FORMAT_NONE }, channels{ 0 }, bits{ 0 }, length{ 0 }
		{}

		string filePath; //File path of the sound.
		string name;	  //Name of the sound. This includes the type.
		FMOD_SOUND_TYPE type; //Type of audio format. eg mp3, wav, ogg.
		FMOD_SOUND_FORMAT format; //Type of format it is in.
		int channels; //Not to be confused with FMOD Channel.
		int bits; //The number of bits of the sound.
		float length; //Duration of sound in seconds.

	};

}