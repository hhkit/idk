#include "stdafx.h"
#include "ClientMoveManager.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <network/SubstreamManager.inl>
#include <network/ConnectionManager.inl>
#include <network/MoveClientMessage.h>
#include <network/GhostFlags.h>
#include <network/ElectronView.h>

namespace idk
{
	static bool quat_equals(const quat& lhs, const quat& rhs)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (abs(lhs[i] - rhs[i]) > std::numeric_limits<float>::epsilon() * 4)
				return false;
		}
		return true;
	}

	void ClientMoveManager::SubscribeEvents(ClientConnectionManager& client)
	{
		//OnFrameStart(&ClientMoveManager::CachePreviousPositions);
		OnFrameEnd(&ClientMoveManager::SendMoves);
	}

	void ClientMoveManager::SubscribeEvents(ServerConnectionManager&)
	{
		IDK_ASSERT_MSG(false, "Do not attack clientmovemanager to server!");
	}

	void ClientMoveManager::CachePreviousPositions(span<ElectronView> views)
	{
	}

	void ClientMoveManager::SendMoves(span<ElectronView> views)
	{
		for (auto& elem : views)
		{
			if (auto* move_data = std::get_if<ElectronView::ClientObject>(&elem.network_data))
			{
				auto& tfm = *elem.GetGameObject()->Transform(); 
				
				connection_manager->CreateAndSendMessage<MoveClientMessage>(GameChannel::FASTEST_GUARANTEED, [&](MoveClientMessage& msg)
					{
						msg.state_mask = elem.state_mask;
						msg.network_id = elem.network_id;
						msg.pack = elem.PackMoveData();
					});
			}
		}
	}
}
