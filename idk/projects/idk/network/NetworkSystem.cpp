#include "pch_common.h"
#include "NetworkSystem.h"

namespace idk
{
	void NetworkSystem::InstantiateServer()
	{
	}
	bool NetworkSystem::IsHost()
	{
		return static_cast<bool>(lobby);
	}
	void NetworkSystem::Init()
	{
		yojimbo::Server;
	}
	void NetworkSystem::Shutdown()
	{
	}
}