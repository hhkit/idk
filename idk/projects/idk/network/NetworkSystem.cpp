#include "pch_common.h"
#include <app/Application.h>
#include "NetworkSystem.h"
#include "Server.h"
#include "Client.h"
#include <yojimbo/yojimbo.h>
namespace idk
{
	NetworkSystem::NetworkSystem() = default;
	NetworkSystem::~NetworkSystem() = default;

	void NetworkSystem::InstantiateServer(const Address& d)
	{
		ResetNetwork();
		lobby = std::make_unique<Server>(Address{d.a,d.b,d.c,d.d, server_listen_port});
	}

	void NetworkSystem::ConnectToServer(const Address& d)
	{
		ResetNetwork();
		client = std::make_unique<Client>(Address{ d.a,d.b,d.c,d.d, server_listen_port });
	}

	bool NetworkSystem::IsHost()
	{
		return static_cast<bool>(lobby);
	}

	void NetworkSystem::ReceivePackets()
	{
		if (lobby)
			lobby->ReceivePackets();
		if (client)
			client->ReceivePackets();
	}

	void NetworkSystem::SendPackets()
	{
		if (lobby)
			lobby->SendPackets();

		if (client)
			client->SendPackets();
	}

	void NetworkSystem::Init()
	{
		InitializeYojimbo();
	}

	void NetworkSystem::LateInit()
	{
		for (auto& device : Core::GetSystem<Application>().GetNetworkDevices())
		{
			LOG_TO(LogPool::NETWORK, "Found %s device (%s) with address %s.", device.name.c_str(), device.fullname.c_str(), string{ device.ip_addresses.front() }.c_str());
		}
	}

	void NetworkSystem::Shutdown()
	{
		ShutdownYojimbo();
	}
	void NetworkSystem::ResetNetwork()
	{
		if (lobby)
			lobby.reset();
		if (client)
			client.reset();
	}
}