#include "stdafx.h"
#include "EventManager.h"
#include <prefab/Prefab.h>
#include <scene/Scene.h>
#include <scene/SceneManager.h>
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <network/NetworkSystem.h>
#include <network/Server.h>
#include <network/events/InstantiatePrefabPayload.h>
namespace idk
{
	bool EventManager::SendEvent(const EventInstantiatePrefabPayload& event)
	{
		if (Core::GetSystem<NetworkSystem>().IsHost())
		{
			auto obj = event.prefab->Instantiate(*Core::GetSystem<SceneManager>().GetActiveScene());
			auto& tfm = *obj->Transform();
			if (event.has_position)
				tfm.GlobalPosition(event.position);
			if (event.has_rotation)
				tfm.GlobalRotation(event.rotation);

			// send upwards
			Core::GetSystem<NetworkSystem>().GetServer().SendEvent(event);
			return true;
		}
		return false;
	}
}
