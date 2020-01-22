#pragma once
#include "ServerConnectionManager.h"
#include <network/Server.h>

namespace idk
{
	template<typename Func, typename>
	inline void ServerConnectionManager::Subscribe(GameMessageType type, Func&& func)
	{
		OnMessageReceived_slots.push_back(EventSlot{ type, server.OnMessageReceived[clientID][(int)type].Listen(std::forward<Func>(func)) });
	}
}