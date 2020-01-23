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
	template<typename T>
	inline T* ServerConnectionManager::CreateMessage()
	{
		return server.CreateMessage<T>(clientID);
	}
	template<typename T>
	inline void ServerConnectionManager::SendMessage(T* message, bool guarantee_delivery)
	{
		server.SendMessage(clientID, message, guarantee_delivery);
	}
	template<typename Manager>
	inline Manager* ServerConnectionManager::GetManager()
	{
		for (auto& elem : substream_managers)
		{
			if (elem->GetManagerType() == index_in_tuple_v<Manager, SubstreamTypes>)
				return elem.get();
		}
		return nullptr;
	}
}