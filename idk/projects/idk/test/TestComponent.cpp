#include "stdafx.h"
#include "TestComponent.h"
#include <app/Application.h>
#include <network/NetworkSystem.h>
#include <network/Client.h>

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
	}
}
