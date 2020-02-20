#include "stdafx.h"
#include "Server.h"
#include <event/Signal.inl>
#include <script/ScriptSystem.h>
#include <network/NetworkSystem.h>
#include <core/Handle.inl>
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
		server.AdvanceTime(server.GetTime() + Core::GetRealDT().count());
		server.ReceivePackets();
		ProcessMessages();
	}

	void Server::SendPackets()
	{
		server.SendPackets();
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
		for (auto& target : Core::GetSystem<NetworkSystem>().GetCallbackTargets())
		{
			mono::ManagedObject obj{"Player"};
			obj.Assign("connectionId", clientIndex);
			if (auto type = target->GetObject().Type())
			{
				if (auto thunk = type->GetThunk("OnConnectedToServer"))
					thunk->Invoke(target->GetObject().Raw(), obj.Raw());
			}
		}
	}

	void Server::ClientDisconnected(int clientIndex)
	{
		LOG_TO(LogPool::NETWORK, "Client %d disconnected", clientIndex);
		OnClientDisconnect.Fire(clientIndex);
		for (auto& target : Core::GetSystem<NetworkSystem>().GetCallbackTargets())
		{
			auto type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Player");
			auto obj  = mono_object_new(mono_domain_get(), type->Raw());
			auto method = mono_class_get_method_from_name(type->Raw(), ".ctor", 1);
			void* args[] = { &clientIndex, nullptr };
			
			if (auto type = target->GetObject().Type())
			{
				if (auto thunk = type->GetThunk("OnDisconnectedFromServer"))
					thunk->Invoke(target->GetObject().Raw(), mono_runtime_invoke(method, obj, args, nullptr));
			}
		}
	}
}