#pragma once
#include <yojimbo/yojimbo.h>
#include "Adapter.h"
#include "GameConfiguration.h"
#include "Address.h"

namespace idk
{
	class ClientConnectionManager;

	class Client
	{
	public:
		Signal<> OnClientConnected;
		Signal<> OnClientDisconnected;
		Signal<yojimbo::Message*> OnMessageReceived[(int)GameMessageType::COUNT];

		Client(const Address& addr);
		~Client();

		void ProcessMessages();
		void ReceivePackets();
		void SendPackets();

		void SendMessage(yojimbo::Message* message, bool guarantee_delivery = false);
	private:
		Adapter           adapter;
		GameConfiguration config;
		yojimbo::Client   client;
		bool connected_last_frame = false;

		void ProcessMessage(yojimbo::Message* message);
	};
}