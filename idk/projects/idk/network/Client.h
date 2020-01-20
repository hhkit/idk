#pragma once
#include <yojimbo/yojimbo.h>
#include "Adapter.h"
#include "GameConfiguration.h"
#include "Address.h"

namespace idk
{
	class Client
	{
	public:
		Client(const Address& addr);
		~Client();

		void ProcessMessages();
		void ReceivePackets();
		void SendPackets();

		void SendTestMessage(int i);
	private:
		Adapter           adapter;
		GameConfiguration config;
		yojimbo::Client   client;

		void ProcessMessage(yojimbo::Message* message);
	};
}