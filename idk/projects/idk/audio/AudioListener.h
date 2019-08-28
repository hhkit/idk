//////////////////////////////////////////////////////////////////////////////////
//@file		AudioListener.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	A GameObject Component that listens for audio. There is normally one
//			listener at any given point.

//////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <IncludeComponents.h>

namespace idk
{
	class AudioListener : public Component<AudioListener> {


	public:

		void SetAsActive();

		void UpdateListenerPosition();

	};

}