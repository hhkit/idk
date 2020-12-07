#pragma once
#include "GameConfiguration.h"
#include <network/Message.h>
#include <steam/steam_api_common.h>

#undef SendMessage

namespace idk
{
	class ClientConnectionManager;

	class Client
	{
	public:
		Signal<Message*> OnMessageReceived[MessageCount];

		Client(CSteamID lobby_id);
		~Client();

		CSteamID GetLobbyID() { return lobby_id; }
		//void ProcessMessages();
		void ProcessMessage(Message* message, uint32_t id);
		//void ReceivePackets();
		void SendPackets();

		void SetPacketLoss(float percent_loss);
		void SetLatency(seconds dur);

		//float GetRTT();

		void SendMessage(SteamNetworkingMessage_t* message);

	private:
		CSteamID lobby_id;
		std::vector<SteamNetworkingMessage_t*> out_messages;
	};
}