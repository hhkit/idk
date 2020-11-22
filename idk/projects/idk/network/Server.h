#pragma once
#include "GameConfiguration.h"
#include <event/Signal.h>
#include <network/Message.h>
#include <steam/steam_api_common.h>
#pragma warning(push)
#pragma warning(disable:4996)
#include <steam/isteammatchmaking.h>
#pragma warning(pop)
#include <steam/steamnetworkingtypes.h>

#undef SendMessage

namespace idk
{
	class EventManager;
	class EventInstantiatePrefabPayload; //TODO: resolve this

	class Server
	{
	public:
		static constexpr auto ALL_CLIENTS = -1;

		// signals
		Signal<Message*> OnMessageReceived[GameConfiguration::MAX_CLIENTS][MessageCount];

		// ctor
		Server(CSteamID lobby_id);
		~Server();

		CSteamID GetLobbyID() { return lobby_id; }
		void SetPacketLoss(float percent_loss);
		void SetLatency(seconds dur);
		float GetRTT(int clientIndex) const;

		void EvictClient(int clientId);

		void ProcessMessage(int clientIndex, Message* message, uint32_t id);
		void SendPackets();

		void SendMessage(SteamNetworkingMessage_t* message);

		void ClientConnected(int clientIndex);
		void ClientDisconnected(int clientIndex);

	private:
		CSteamID lobby_id;
		std::vector<SteamNetworkingMessage_t*> out_messages;
	};
}