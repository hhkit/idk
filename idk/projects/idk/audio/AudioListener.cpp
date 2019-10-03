//////////////////////////////////////////////////////////////////////////////////
//@file		AudioListener.cpp
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		18 AUG 2019
//@brief	A GameObject Component that listens for audio. There is normally one
//			Listener at any given point.

//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h" //Needed for every CPP. Precompiler
#include <audio/AudioListener.h>
#include <audio/AudioSystem.h>
#include <scene/SceneManager.h>


namespace idk
{
	void AudioListener::SetAsActive()
	{
		AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();
		audioSystem.SetMainAudioListener(GetHandle());
		is_active = true;
	}
	void AudioListener::UpdateListenerPosition()
	{
		if (const Handle<GameObject> parent = GetGameObject()) 
		{
			if (const Handle<Transform> transform = parent->GetComponent<Transform>()) 
			{
				AudioSystem& audioSystem = Core::GetSystem<AudioSystem>();

				if (const Handle<RigidBody> rigidbody = parent->GetComponent<RigidBody>())
					audioSystem.Set3DListenerAttributes(transform->position, rigidbody->velocity(), transform->Forward(), transform->Up());
				else
					audioSystem.Set3DListenerAttributes(transform->position, vec3{}, transform->Forward(), transform->Up());

			}
		}
	}
}