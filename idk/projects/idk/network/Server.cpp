#include "stdafx.h"
#include "Server.h"
#include <event/Signal.inl>
#include <script/ScriptSystem.h>
#include <network/NetworkSystem.h>
#include <core/Handle.inl>
#include <core/Scheduler.h>
#include <script/MonoBehavior.h>
#include <script/ManagedObj.inl>

#undef SendMessage

namespace idk
{
	Server::Server(const Address& address)
		: adapter{ this }, server{ yojimbo::GetDefaultAllocator(), DEFAULT_PRIVATE_KEY, yojimbo::Address{address.a, address.b, address.c, address.d, address.port}, config, adapter, 0.0 }
	{
		server.Start(GameConfiguration::MAX_CLIENTS);
		if (!server.IsRunning())
		{
			LOG_CRASH_TO(LogPool::NETWORK, "Could not create server!");
			throw;
		}

		char buffer[256];
		server.GetAddress().ToString(buffer, sizeof(buffer));
		LOG_TO(LogPool::NETWORK, "Server address is %s", buffer);
	}
	Server::~Server()
	{
		server.DisconnectAllClients();
		server.Stop();
	}
	void Server::ProcessMessage(int clientIndex, yojimbo::Message* message)
	{
		OnMessageReceived[clientIndex][message->GetType()].Fire(message);
	}

	void Server::ProcessMessages()
	{
		for (int i = 0; i < GameConfiguration::MAX_CLIENTS; i++) 
		{
			if (server.IsClientConnected(i)) 
			{
				for (int j = 0; j < config.numChannels; j++) 
				{
					while (auto message = server.ReceiveMessage(i, j)) 
					{
						ProcessMessage(i, message);
						server.ReleaseMessage(i, message);
					}
				}
			}
		}
	}
	void Server::ReceivePackets()
	{
		server.AdvanceTime(server.GetTime() + Core::GetScheduler().GetNetworkTick().count());
		server.ReceivePackets();
		ProcessMessages();
	}

	void Server::SendPackets()
	{
		server.SendPackets();
	}
	float Server::GetRTT(int clientIndex) const
	{
		yojimbo::NetworkInfo info;
		server.GetNetworkInfo(clientIndex, info);
		return info.RTT;
	}


	void Server::SendMessage(int clientIndex, yojimbo::Message* message, GameChannel delivery_mode)
	{
		if (clientIndex == ALL_CLIENTS)
		{
			for (int i = 0; i < server.GetNumConnectedClients(); ++i)
				server.SendMessage(i, (int)(delivery_mode), message);
		}
		else
		{
			server.SendMessage(clientIndex, (int)(delivery_mode), message);
		}
	}

	void Server::ClientConnected(int clientIndex)
	{
		LOG_TO(LogPool::NETWORK, "Client %d connected", clientIndex);
		OnClientConnect.Fire(clientIndex);

		auto player_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Client");
		auto player = player_type->ConstructTemporary(clientIndex);
		for (auto& target : Core::GetSystem<NetworkSystem>().GetCallbackTargets())
		{
			if (auto type = target->GetObject().Type())
			{
				if (auto thunk = type->GetThunk("OnClientConnect"))
					thunk->Invoke(target->GetObject().Raw(), player);
			}
		}
	}

	void Server::ClientDisconnected(int clientIndex)
	{
		LOG_TO(LogPool::NETWORK, "Client %d disconnected", clientIndex);
		OnClientDisconnect.Fire(clientIndex);

		auto player_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Client");
		auto player = player_type->ConstructTemporary(clientIndex);
		for (auto& target : Core::GetSystem<NetworkSystem>().GetCallbackTargets())
		{
			if (auto type = target->GetObject().Type())
			{
				if (auto thunk = type->GetThunk("OnClientDisconnect"))
					thunk->Invoke(target->GetObject().Raw(), player);
			}
		}
	}
}