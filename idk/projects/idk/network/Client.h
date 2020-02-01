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
		Signal<> OnConnectionToServer;
		Signal<> OnDisconnectionFromServer;
		Signal<yojimbo::Message*> OnMessageReceived[MessageCount];

		Client(const Address& addr);
		~Client();

		void ProcessMessages();
		void ReceivePackets();
		void SendPackets();

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