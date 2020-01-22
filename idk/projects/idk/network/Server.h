#pragma once
#include <yojimbo/yojimbo.h>
#include <event/Signal.h>
#include "Adapter.h"
#include "GameConfiguration.h"
#include "Address.h"
namespace idk
{
	class EventManager;
	class EventInstantiatePrefabPayload; //TODO: resolve this

	class Server
	{
	public:
		static constexpr auto ALL_CLIENTS = -1;

		// signals
		Signal<int>               OnClientConnect;
		Signal<int>               OnClientDisconnect;
		Signal<yojimbo::Message*> OnMessageReceived[GameConfiguration::MAX_CLIENTS][(int)GameMessageType::COUNT];

		// ctor
		Server(const Address& address);
		~Server();

		void ProcessMessages();

		void ReceivePackets();
		void SendPackets();
		void SendMessage(int clientIndex, yojimbo::Message* message, bool guarantee_delivery = false);

		// callbacks
		void ClientConnected(int clientIndex);
		void ClientDisconnected(int clientIndex);
	private:
		Adapter           adapter;
		GameConfiguration config;
		yojimbo::Server   server;

		void ProcessMessage(int clientIndex, yojimbo::Message* message);
	};
}