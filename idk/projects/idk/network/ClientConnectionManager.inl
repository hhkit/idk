#pragma once
#include "ClientConnectionManager.h"
#include <network/Client.h>
namespace idk
{
	template<typename Func, typename>
	void ClientConnectionManager::Subscribe(GameMessageType type, Func&& func)
	{
		OnMessageReceived_slots.emplace_back(client.OnMessageReceived[(int)type].Listen(std::forward<Func>(func)));
	}
}