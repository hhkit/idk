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
#include <network/NetworkTuple.inl>
#include <meta/variant.inl>
#include <steam/isteamnetworkingsockets.h>
#undef SendMessage

namespace idk
{
	namespace detail
	{
		using NetworkHelper = NetworkTuple<NetworkMessageTuple>;
	}

	Client::Client(CSteamID lobby_id)
		: lobby_id{ lobby_id }
	{
	}

	Client::~Client()
	{

	}

	//void Client::ProcessMessages()
	//{
	//	for (int i = 0; i < config.numChannels; i++) 
	//	{
	//		while (auto message = client.ReceiveMessage(i)) 
	//		{
	//			ProcessMessage(message);
	//			client.ReleaseMessage(message);
	//		}
	//	}

	//}

	//void Client::ReceivePackets()
	//{
	//	client.AdvanceTime(client.GetTime() + Core::GetScheduler().GetNetworkTick().count() );
	//	client.ReceivePackets();

	//	// get rtt

	//	auto connected_this_frame = client.IsConnected();
	//	if (connected_this_frame && !connected_last_frame)
	//	{
	//		OnConnectionToServer.Fire();
	//		for (auto& target : Core::GetSystem<NetworkSystem>().GetCallbackTargets())
	//			target->FireMessage("OnConnectedToServer");
	//	}
	//	if (!connected_this_frame && connected_last_frame)
	//	{
	//		OnDisconnectionFromServer.Fire();
	//		for (auto& target : Core::GetSystem<NetworkSystem>().GetCallbackTargets())
	//			target->FireMessage("OnDisconnectedFromServer");
	//	}
	//	if (connected_this_frame)
	//		ProcessMessages();

	//	connected_last_frame = connected_this_frame;
	//}
	void Client::SendPackets()
	{
		SteamNetworkingSockets()->SendMessages(static_cast<int>(out_messages.size()), out_messages.data(), nullptr);
		out_messages.clear();
	}

	void Client::SetPacketLoss(float percent_loss)
	{
		//client.SetPacketLoss(percent_loss);
	}

	void Client::SetLatency(seconds dur)
	{
		//client.SetLatency(duration_cast<std::chrono::duration<float, std::milli>>(dur).count());
	}

	//float Client::GetRTT()
	//{
	//	yojimbo::NetworkInfo info;
	//	client.GetNetworkInfo(info);
	//	return info.RTT;
	//}

	//yojimbo::Message* Client::CreateMessage(int id)
	//{
	//	return client.CreateMessage(id);
	//}

	void Client::SendMessage(SteamNetworkingMessage_t* message)
	{
		out_messages.push_back(message);
	}

	void Client::ProcessMessage(Message* message, uint32_t id)
	{
		constexpr auto message_name_array = detail::NetworkHelper::GenNames();
		if (id != index_in_tuple_v<GhostMessage, NetworkMessageTuple>
			&& id != index_in_tuple_v<GhostAcknowledgementMessage, NetworkMessageTuple>
			&& id != index_in_tuple_v<MoveClientMessage, NetworkMessageTuple>
			)
		LOG_TO(LogPool::NETWORK, "Received %s message", message_name_array[id].data());
		OnMessageReceived[id].Fire(message);
	}
}