#include "pch_common.h"
#include <yojimbo/yojimbo.h>

#include "NetworkSystem.h"

#include <app/Application.h>
#include <network/Server.h>
#include <network/Client.h>
#include <network/ServerConnectionManager.h>
#include <network/ClientConnectionManager.h>
#include <network/IDManager.h>
#include <network/ElectronView.h>
#include <core/GameState.h>
namespace idk
{
	NetworkSystem::NetworkSystem() = default;
	NetworkSystem::~NetworkSystem() = default;

	void NetworkSystem::InstantiateServer(const Address& d)
	{
		ResetNetwork();
		lobby = std::make_unique<Server>(Address{d.a,d.b,d.c,d.d, server_listen_port});
		lobby->OnClientConnect += [this](int clientid)
		{
			server_connection_manager[clientid] = std::make_unique<ServerConnectionManager>(clientid, *lobby);
		};
		lobby->OnClientDisconnect += [this](int clientid)
		{
			server_connection_manager[clientid].reset();
		};
		id_manager = std::make_unique<IDManager>();
	}

	void NetworkSystem::ConnectToServer(const Address& d)
	{
		ResetNetwork();
		client = std::make_unique<Client>(Address{ d.a,d.b,d.c,d.d, server_listen_port });
		client->OnConnectionToServer += [this]()
		{
			client_connection_manager = std::make_unique<ClientConnectionManager>(*client);
		};
		client->OnDisconnectionFromServer += [this]()
		{
			client_connection_manager.reset();
		};
		id_manager = std::make_unique<IDManager>();
	}

	bool NetworkSystem::IsHost()
	{
		return static_cast<bool>(lobby);
	}

	ConnectionManager& NetworkSystem::GetConnectionManager(size_t token)
	{
		if (token == GameConfiguration::MAX_CLIENTS)
		{
			if (client_connection_manager)
				return *client_connection_manager;
			for (auto& elem : server_connection_manager)
				if (elem)
					return *elem;

			throw;
		}
		return *server_connection_manager[token];
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
		lobby.reset();
		for (auto& elem : server_connection_manager)
			elem.reset();
		client.reset();
		client_connection_manager.reset();
		id_manager.reset();

		// network ids no longer relevant
		for (auto& elem : Core::GetGameState().GetObjectsOfType<ElectronView>())
			elem.network_id = 0;
	}
}