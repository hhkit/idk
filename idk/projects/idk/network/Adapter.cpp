#include "stdafx.h"
#include "Adapter.h"
#include "Server.h"
#include <network/messages/TestMessage.h>
#include <network/messages/EventMessage.h>

namespace idk
{
	YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, (int)GameMessageType::COUNT);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::TEST, TestMessage);
	YOJIMBO_DECLARE_MESSAGE_TYPE((int)GameMessageType::EVENT_INSTANTIATE_PREFAB, EventInstantiatePrefabMessage);
	YOJIMBO_MESSAGE_FACTORY_FINISH();

	yojimbo::MessageFactory* Adapter::CreateMessageFactory(yojimbo::Allocator& allocator)
	{
		return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
	}
	void Adapter::OnServerClientConnected(int clientIndex)
	{
		server->ClientConnected(clientIndex);
	}

	void Adapter::OnServerClientDisconnected(int clientIndex)
	{
		server->ClientDisconnected(clientIndex);
	}

}