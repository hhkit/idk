#include "stdafx.h"
#include "GhostManager.h"
#include <network/GhostMessage.h>
#include <network/NetworkSystem.h>
#include <network/IDManager.h>
#include <network/ElectronTransformView.h>
#include <network/ClientConnectionManager.inl>

namespace idk
{
	void GhostManager::SubscribeEvents(ClientConnectionManager& client)
	{
		client.Subscribe<GhostMessage>([this](GhostMessage* msg) { OnGhostReceived(msg); });
	}

	void GhostManager::SubscribeEvents(ServerConnectionManager& server)
	{

	}

	void GhostManager::SendGhosts(span<ElectronTransformView> views)
	{
		for (auto& elem : views)
			elem.UpdateGhost();

		for (auto& elem : views)
		{
			
		}
	}

	void GhostManager::OnGhostReceived(GhostMessage* msg)
	{
		auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
		auto view = id_man.GetViewFromId(msg->network_id);
		if (view)
		{
			// push the ghost data into the view
		}
	}

}