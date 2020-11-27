#include "stdafx.h"
#include "Server.h"
#include <event/Signal.inl>
#include <script/ScriptSystem.h>
#include <network/NetworkSystem.h>
#include <core/Handle.inl>
#include <core/Scheduler.h>
#include <script/MonoBehavior.h>
#include <script/ManagedObj.inl>
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

	Server::Server(CSteamID lobby_id)
		: lobby_id{ lobby_id }
	{
	}
	Server::~Server()
	{
		// have to disconnect all clients as well
		if (lobby_id.IsValid())
			SteamMatchmaking()->LeaveLobby(lobby_id);
	}

	void Server::ProcessMessage(int clientIndex, Message* message, uint32_t id)
	{
		constexpr auto message_name_array = detail::NetworkHelper::GenNames();
		if (id != index_in_tuple_v<GhostMessage, NetworkMessageTuple>
			&& id != index_in_tuple_v<GhostAcknowledgementMessage, NetworkMessageTuple>
			&& id != index_in_tuple_v<MoveClientMessage, NetworkMessageTuple>
			)
			LOG_TO(LogPool::NETWORK, "Received %s message from client %d", message_name_array[id].data(), clientIndex);

		OnMessageReceived[clientIndex][id].Fire(message);
	}

	//void Server::ProcessMessages()
	//{
	//	for (int i = 0; i < GameConfiguration::MAX_CLIENTS; i++) 
	//	{
	//		if (server.IsClientConnected(i)) 
	//		{
	//			for (int j = 0; j < config.numChannels; j++) 
	//			{
	//				while (auto message = server.ReceiveMessage(i, j)) 
	//				{
	//					ProcessMessage(i, message);
	//					server.ReleaseMessage(i, message);
	//				}
	//			}
	//		}
	//	}
	//}
	//void Server::ReceivePackets()
	//{
	//	server.AdvanceTime(server.GetTime() + Core::GetScheduler().GetNetworkTick().count());
	//	server.ReceivePackets();
	//	ProcessMessages();
	//}

	void Server::SendPackets()
	{
		SteamNetworkingSockets()->SendMessages(static_cast<int>(out_messages.size()), out_messages.data(), nullptr);
		out_messages.clear();
	}

	void Server::SetPacketLoss(float percent_loss)
	{
		//server.SetPacketLoss(percent_loss);
	}

	void Server::SetLatency(seconds dur)
	{
		//server.SetLatency(duration_cast<std::chrono::duration<float, std::milli>>(dur).count());
	}

	//float Server::GetRTT(int clientIndex) const
	//{
	//	yojimbo::NetworkInfo info;
	//	server.GetNetworkInfo(clientIndex, info);
	//	return info.RTT;
	//}


	void Server::SendMessage(SteamNetworkingMessage_t* message)
	{
		out_messages.push_back(message);
	}

}