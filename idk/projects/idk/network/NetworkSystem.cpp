#include "pch_common.h"
#include "NetworkSystem.h"
#include "Server.h"
#include <yojimbo/yojimbo.h>
namespace idk
{
	NetworkSystem::NetworkSystem() = default;
	NetworkSystem::~NetworkSystem() = default;

	void NetworkSystem::InstantiateServer()
	{
		lobby = std::make_unique<Server>(Address{127,0,0,1, 8000});
	}

	bool NetworkSystem::IsHost()
	{
		return static_cast<bool>(lobby);
	}

	void NetworkSystem::ReceivePackets()
	{
		if (lobby)
			lobby->ReceivePackets();
	}

	void NetworkSystem::SendPackets()
	{
		if (lobby)
			lobby->SendPackets();
	}

	void NetworkSystem::Init()
	{
		InitializeYojimbo();
	}

	void NetworkSystem::Shutdown()
	{
		ShutdownYojimbo();
	}
}