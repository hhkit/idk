#include "Adapter.h"
#include "Server.h"
namespace idk
{

	yojimbo::MessageFactory* Adapter::CreateMessageFactory(yojimbo::Allocator& allocator)
	{
		return nullptr;
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