#include "stdafx.h"
#include "EventManager.h"
#include <network/ClientConnectionManager.inl>
#include <network/ServerConnectionManager.inl>
#include <network/TestMessage.h>
#include <iostream>

namespace idk
{
	void EventManager::SubscribeEvents(ClientConnectionManager& client)
	{
		this->client = &client;
		client.Subscribe<TestMessage>([](TestMessage* message)
			{
				LOG_TO(LogPool::NETWORK, "Received message %d", message->i);
			});
	}

	void EventManager::SubscribeEvents(ServerConnectionManager& server)
	{
		this->server = &server;
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
		if (client)
		{
			auto test_msg = client->CreateMessage<TestMessage>();
			test_msg->i = i;
			client->SendMessage(test_msg, true);
		}
	}
}
