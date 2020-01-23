#pragma once
#include "ServerConnectionManager.h"
#include <network/Server.h>
#include <network/SubstreamManager.h>

namespace idk
{
	template<typename Message, typename Func, typename>
	void ServerConnectionManager::Subscribe(Func&& func)
	{
		OnMessageReceived_slots.push_back(EventSlot{ MessageID<Message>,
			server.OnMessageReceived[clientID][MessageID<Message>].Listen([fn = std::forward<Func>(func)](yojimbo::Message* message)
			{
				return fn(static_cast<Message*>(message));
			})
			}
		);
	}
}