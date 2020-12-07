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

	void Client::SendMessage(SteamNetworkingMessage_t* message)
	{
		out_messages.push_back(message);
	}

	void Client::ProcessMessage(Message* message, uint32_t id)
	{
		//constexpr auto message_name_array = detail::NetworkHelper::GenNames();
		//if (id != index_in_tuple_v<GhostMessage, NetworkMessageTuple>
		//	&& id != index_in_tuple_v<GhostAcknowledgementMessage, NetworkMessageTuple>
		//	&& id != index_in_tuple_v<MoveClientMessage, NetworkMessageTuple>
		//	)
		//	LOG_TO(LogPool::NETWORK, "Received %s message", message_name_array[id].data());
		OnMessageReceived[id].Fire(message);
	}
}