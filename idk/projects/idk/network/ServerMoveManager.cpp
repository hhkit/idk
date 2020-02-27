#include "stdafx.h"
#include "ServerMoveManager.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <network/MoveClientMessage.h>
#include <network/ServerConnectionManager.inl>
#include <network/IDManager.h>
#include <network/NetworkSystem.h>
#include <network/ElectronView.h>
#include <network/ElectronTransformView.h>
namespace idk
{
	void ServerMoveManager::SubscribeEvents(ClientConnectionManager&)
	{
		IDK_ASSERT_MSG(false, "Server Move Manager only used on server!");
	}

	void ServerMoveManager::SubscribeEvents(ServerConnectionManager& server)
	{
		server.Subscribe<MoveClientMessage>([this](MoveClientMessage& msg) { OnMoveReceived(msg); });
	}

	void ServerMoveManager::OnMoveReceived(MoveClientMessage& move)
	{
		//LOG_TO(LogPool::NETWORK, "Received move event");
		auto view = Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(move.network_id);
		if (view)
		{
			view->state_mask = move.state_mask;
			view->UnpackMoveData(move.sequence_number, move.pack);
		}
	}
}
