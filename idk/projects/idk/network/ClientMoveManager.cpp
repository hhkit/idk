#include "stdafx.h"
#include "ClientMoveManager.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <network/SubstreamManager.inl>
#include <network/ConnectionManager.inl>
#include <network/MoveClientMessage.h>
#include <network/NetworkSystem.inl>
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

	void ClientMoveManager::SubscribeEvents(ClientConnectionManager&)
	{
		//OnFrameStart(&ClientMoveManager::CachePreviousPositions);
		Core::GetSystem<NetworkSystem>().SubscribePacketResponse(&ClientMoveManager::CachePreviousPositions);
		OnFrameEnd(&ClientMoveManager::SendMoves);
	}

	void ClientMoveManager::SubscribeEvents(ServerConnectionManager&)
	{
		IDK_ASSERT_MSG(false, "Do not attack clientmovemanager to server!");
	}

	void ClientMoveManager::CachePreviousPositions(span<ElectronView> views)
	{
		for (auto& elem : views)
			elem.UpdateClient();
	}

	void ClientMoveManager::SendMoves(span<ElectronView> views)
	{
		for (auto& elem : views)
		{
			if (auto* move_data = std::get_if<ElectronView::ClientObject>(&elem.move_state))
			{
				
				if (elem.state_mask)
				{
					LOG_TO(LogPool::NETWORK, "Sending client move message for %d", elem.network_id);
					connection_manager->CreateAndSendMessage<MoveClientMessage>(GameChannel::FASTEST_GUARANTEED, [&](MoveClientMessage& msg)
						{
							msg.sequence_number = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
							msg.state_mask = elem.state_mask;
							msg.network_id = elem.network_id;
							msg.pack = elem.PackMoveData();
						});
				}
			}
		}
	}
}
