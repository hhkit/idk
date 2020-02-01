#include "stdafx.h"
#include "TestComponent.h"
#include <app/Application.h>
#include <network/NetworkSystem.h>
#include <network/ConnectionManager.inl>
#include <network/EventManager.h>
#include <core/GameObject.h>
#include <common/Transform.h>
namespace idk
{
	void TestComponent::NetworkUpdate()
	{
		auto& app_sys = Core::GetSystem<Application>();
		auto& network_sys = Core::GetSystem<NetworkSystem>();

		if (app_sys.GetKeyDown(Key::T))
		{
			static int i = 0;
			if (auto conn_man = network_sys.GetConnectionTo(Host::SERVER))
				conn_man->GetManager<EventManager>()->SendTestMessage(i);
			LOG("Sending test message %d", i);
			++i;
		}

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
