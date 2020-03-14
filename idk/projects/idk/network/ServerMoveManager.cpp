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
#include <serialize/binary.inl>

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
		const auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
		for (auto& move_pack : move.move_packs)
		{
			if (auto view = id_man.GetViewFromId(move_pack.network_id))
			{
				view->UnpackMoveData(move_pack);
			}
		}
	}
}
