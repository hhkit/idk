#include "stdafx.h"
#include "ConnectionManager.h"
#include <network/NetworkSystem.h>
#include <network/Server.h>
#include <network/Client.h>
#include <network/ServerConnectionManager.inl>
#include <network/ClientConnectionManager.inl>

namespace idk
{
	ConnectionManager::~ConnectionManager() = default;

	bool ConnectionManager::IsHost()
	{
		return Core::GetSystem<NetworkSystem>().IsHost();
	}
	void ConnectionManager::Init()
	{
		// when a server is created, subscribe to client event
		Core::GetSystem<NetworkSystem>().OnServerCreate += [this]()
		{
			auto& server = Core::GetSystem<NetworkSystem>().GetServer();
			// when a client is connected, create a server connection manager
			server.OnClientConnect += [&server, this](int clientID)
			{
				server_connection_manager[clientID] = std::make_unique<ServerConnectionManager>(clientID, server);
			};
			server.OnClientDisconnect += [&server, this](int clientID)
			{
				server_connection_manager[clientID].reset();
			};
		};

		// when a client is created, subscribe to connected event
		Core::GetSystem<NetworkSystem>().OnClientCreate += [this]()
		{
			auto& client = Core::GetSystem<NetworkSystem>().GetClient();
			client.OnClientConnected += [&client , this]()
			{
				client_connection_manager = std::make_unique<ClientConnectionManager>(client);
			};
			client.OnClientDisconnected += [&client, this]
			{
				client_connection_manager.reset();
			};
		};
	}

	void ConnectionManager::Shutdown()
	{
		client_connection_manager.reset();
		for (auto& elem : server_connection_manager)
			elem.reset();
	}
}