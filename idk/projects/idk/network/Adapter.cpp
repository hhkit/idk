#include "stdafx.h"
#include "Adapter.h"
#include "Server.h"
#include <network/IncludeMessages.h>

namespace idk
{
#define ADD_MESSAGE(TYPE) YOJIMBO_DECLARE_MESSAGE_TYPE(MessageID<TYPE>, TYPE)

	YOJIMBO_MESSAGE_FACTORY_START(GameMessageFactory, MessageCount);
	ADD_MESSAGE(TestMessage);
	//ADD_MESSAGE(EventInstantiatePrefabMessage);
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