#include "stdafx.h"
#include "TestComponent.h"
#include <app/Application.h>
#include <network/NetworkSystem.h>
#include <network/ConnectionManager.inl>
#include <network/EventManager.h>
#include <network/ElectronView.inl>
#include <network/ack_utils.h>
#include <core/GameObject.h>
#include <common/Transform.h>
namespace idk
{
	void TestComponent::NetworkUpdate()
	{
		auto& app_sys = Core::GetSystem<Application>();
		auto& network_sys = Core::GetSystem<NetworkSystem>();

		if (app_sys.GetKeyDown(Key::P))
		{
			EventManager::BroadcastInstantiatePrefab(makeme,
				send_pos ? opt<vec3>{GetGameObject()->Transform()->GlobalPosition()} : std::nullopt,
				send_rot ? opt<quat>{GetGameObject()->Transform()->GlobalRotation()} : std::nullopt);
		}

		if (app_sys.GetKeyDown(Key::L))
		{
			auto view = GetGameObject()->GetComponent<ElectronView>();
			if (view)
			{
				if (auto conn_man = network_sys.GetConnectionTo(Host::CLIENT0))
					conn_man->GetManager<EventManager>()->SendTransferOwnership(view, Host::CLIENT0);
			}
		}
	}
}
