#pragma once
#include "ClientConnectionManager.h"
#include <network/Client.h>
namespace idk
{
	template<typename Func, typename>
	void ClientConnectionManager::Subscribe(GameMessageType type, Func&& func)
	{
		OnMessageReceived_slots.push_back(EventSlot{ type, client.OnMessageReceived[(int)type].Listen(std::forward<Func>(func)) });
	}
}