#pragma once
#include <yojimbo/yojimbo.h>
#include "Adapter.h"
#include "GameConfiguration.h"
#include "Address.h"
namespace idk
{
	class Server
	{
	public:
		Server(const Address& address);
		~Server();

		void ProcessMessages();

		void ReceivePackets();
		void SendPackets();

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