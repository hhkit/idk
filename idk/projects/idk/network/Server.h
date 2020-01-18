#pragma once
#include <yojimbo/yojimbo.h>
#include "Adapter.h"
#include "GameConfiguration.h"

namespace idk
{
	class Server
	{
	public:
		Server(const yojimbo::Address& address);

		void ProcessMessages();

		void ReceivePackets();
		void SendPackets();

		// callbacks
		void ClientConnected(int clientIndex);
		void ClientDisconnected(int clientIndex);
	private:
		Adapter           adapter;
		yojimbo::Server   server;
		GameConfiguration config;
	};
}