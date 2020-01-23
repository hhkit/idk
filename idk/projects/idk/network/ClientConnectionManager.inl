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
	template<typename T>
	inline T* ClientConnectionManager::CreateMessage()
	{
		return client.CreateMessage<T>();
	}
	template<typename T>
	inline void ClientConnectionManager::SendMessage(T* message, bool guarantee)
	{
		client.SendMessage(message, guarantee);
	}
	template<typename Manager>
	Manager* ClientConnectionManager::GetManager()
	{
		for (auto& elem : substream_managers)
		{
			if (elem->GetManagerType() == index_in_tuple_v<Manager, SubstreamTypes>)
				return static_cast<Manager*>(elem.get());
		}
		return nullptr;
	}
}