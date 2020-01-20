#include "stdafx.h"
#include "TestComponent.h"
#include <app/Application.h>
#include <network/NetworkSystem.h>
#include <network/Client.h>
#include <network/Server.h>
#include <core/GameObject.h>
#include <common/Transform.h>
#include <network/events/InstantiatePrefabPayload.h>
namespace idk
{
	void TestComponent::NetworkUpdate()
	{
		auto& app_sys = Core::GetSystem<Application>();
		auto& network_sys = Core::GetSystem<NetworkSystem>();

		if (app_sys.GetKeyDown(Key::C))
		{
			auto devices = app_sys.GetNetworkDevices();
			auto try_address = Address{ (unsigned char)a,(unsigned char)b,(unsigned char)c,(unsigned char)d };
			LOG_TO(LogPool::NETWORK, "Try connecting to %s", string{ try_address }.c_str());
			network_sys.ConnectToServer(try_address);
		}
		if (app_sys.GetKeyDown(Key::T))
		{
			static int i = 0;
			if (!network_sys.IsHost())
				network_sys.GetClient().SendTestMessage(i++);
		}

		if (app_sys.GetKeyDown(Key::P))
		{
			network_sys.GetServer().SendEvent(EventInstantiatePrefabPayload{
				.prefab = makeme,
				.has_position = send_pos,
				.position = GetGameObject()->Transform()->GlobalPosition(),
				.has_rotation = send_rot,
				.rotation = GetGameObject()->Transform()->GlobalRotation()
				});
		}
	}
}
