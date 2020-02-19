#include "stdafx.h"
#include "Client.h"
#include <core/Core.h>
#include <prefab/Prefab.h>
#include <scene/SceneManager.h>
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <network/NetworkSystem.h>
#include <core/Scheduler.h>
#include <script/ScriptSystem.h>
#include <script/MonoBehavior.h>
#undef SendMessage

namespace idk
{
	Client::Client(const Address& server_addr)
		: client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"), config, adapter, 0.0)
	{
		uint64_t clientId;
		yojimbo::random_bytes((uint8_t*)&clientId, 8);
		client.InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, yojimbo::Address{ server_addr.a, server_addr.b, server_addr.c, server_addr.d, server_addr.port }); 

		LOG_TO(LogPool::NETWORK, "Try connecting to %s", string{ server_addr }.c_str());
		OnConnectionToServer += []() { LOG_TO(LogPool::NETWORK, "Connected to server"); };
		OnDisconnectionFromServer += []() { LOG_TO(LogPool::NETWORK, "Disconnected from server"); };
	}

	Client::~Client()
	{
		client.Disconnect();
	}

	bool Client::IsConnected() const
	{
		return client.IsConnected();
	}

	void Client::ProcessMessages()
	{
		for (int i = 0; i < config.numChannels; i++) 
		{
			while (auto message = client.ReceiveMessage(i)) 
			{
				ProcessMessage(message);
				client.ReleaseMessage(message);
			}
		}

	}

	void Client::ReceivePackets()
	{
		client.AdvanceTime(client.GetTime() + Core::GetRealDT().count());
		client.ReceivePackets();

		// get rtt

		auto connected_this_frame = client.IsConnected();
		if (connected_this_frame && !connected_last_frame)
		{
			OnConnectionToServer.Fire();
			for (auto& target : Core::GetSystem<NetworkSystem>().GetCallbackTargets())
				target->FireMessage("OnDisconnectedFromServer");
		}
		if (!connected_this_frame && connected_last_frame)
		{
			OnDisconnectionFromServer.Fire();
			for (auto& target : Core::GetSystem<NetworkSystem>().GetCallbackTargets())
				target->FireMessage("OnDisconnectedFromServer");
		}
		if (connected_this_frame)
			ProcessMessages();

		connected_last_frame = connected_this_frame;
	}
	void Client::SendPackets()
	{
		client.SendPackets();
	}

	float Client::GetRTT()
	{
		yojimbo::NetworkInfo info;
		client.GetNetworkInfo(info);
		return info.RTT;
	}

	yojimbo::Message* Client::CreateMessage(int id)
	{
		return client.CreateMessage(id);
	}

	void Client::SendMessage(yojimbo::Message* message, GameChannel delivery_mode)
	{
		client.SendMessage((int)(delivery_mode), message);
	}

	void Client::ProcessMessage(yojimbo::Message* message)
	{
		OnMessageReceived[message->GetType()].Fire(message);
	}
}