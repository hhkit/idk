#pragma once
#include "ClientConnectionManager.h"
#include <network/Client.h>
#include <network/SubstreamManager.h>
namespace idk
{
	template<typename Message, typename Func, typename>
	void ClientConnectionManager::Subscribe(Func&& func)
	{
		OnMessageReceived_slots.push_back(EventSlot{ MessageID<Message>, 
			client.OnMessageReceived[MessageID<Message>].Listen([fn = std::forward<Func>(func)](yojimbo::Message* message)
			{
				return fn(static_cast<Message*>(message));
			})
		}
		);
	}
}