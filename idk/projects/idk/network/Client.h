#pragma once
#include <yojimbo/yojimbo.h>
#include "Adapter.h"
#include "GameConfiguration.h"
#include "Address.h"

#undef SendMessage

namespace idk
{
	class ClientConnectionManager;

	class Client
	{
	public:
		Signal<> OnConnectionToServer;
		Signal<> OnDisconnectionFromServer;
		Signal<yojimbo::Message*> OnMessageReceived[MessageCount];

		Client(const Address& addr);
		~Client();

		bool IsConnected() const;
		void ProcessMessages();
		void ReceivePackets();
		void SendPackets();

		void SetPacketLoss(float percent_loss);
		void SetLatency(seconds dur);

		float GetRTT();

		yojimbo::Message* CreateMessage(int id);
		void SendMessage(yojimbo::Message* message, GameChannel delivery_mode);
	private:
		Adapter           adapter;
		GameConfiguration config;
		yojimbo::Client   client;
		bool connected_last_frame = false;

		void ProcessMessage(yojimbo::Message* message);
	};
}