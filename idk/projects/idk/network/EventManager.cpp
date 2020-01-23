#include "stdafx.h"
#include "EventManager.h"
#include <network/ConnectionManager.inl>
#include <network/TestMessage.h>
#include <network/NetworkSystem.h>
#include <iostream>

namespace idk
{
	void EventManager::SubscribeEvents(ClientConnectionManager& client)
	{
		connection_manager = &client;
		client.Subscribe<TestMessage>([](TestMessage* message)
			{
				LOG_TO(LogPool::NETWORK, "Received message %d", message->i);
			});
	}

	void EventManager::SubscribeEvents(ServerConnectionManager& server)
	{
		connection_manager = &server;
		server.Subscribe<TestMessage>([&server](TestMessage* message)
			{
				LOG_TO(LogPool::NETWORK, "Received message %d", message->i);
				auto pingback = server.CreateMessage<TestMessage>();
				pingback->i = message->i + 1;
				server.SendMessage(pingback, true);
			});
	}
	void EventManager::SendTestMessage(int i)
	{
		auto& conn_man = Core::GetSystem<NetworkSystem>().GetConnectionManager();
		auto test_mess = conn_man.CreateMessage<TestMessage>();
		test_mess->i = i;
		conn_man.SendMessage(test_mess, true);
	}
}
