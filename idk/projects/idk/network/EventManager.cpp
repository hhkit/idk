#include "stdafx.h"
#include "EventManager.h"
#include <network/ClientConnectionManager.inl>
#include <iostream>

namespace idk
{
	void EventManager::SubscribeEvents(ClientConnectionManager& client)
	{
		client.Subscribe(GameMessageType::EVENT_INSTANTIATE_PREFAB, [](yojimbo::Message* message)
			{
				std::cout << "what\n";
			});
	}

	void EventManager::SubscribeEvents(ServerConnectionManager& server)
	{
	}
}
