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
	}

	void ClientMoveManager::SubscribeEvents(ServerConnectionManager&)
	{
		IDK_ASSERT_MSG(false, "Do not attack clientmovemanager to server!");
	}

	void ClientMoveManager::SendMoves(span<ElectronView> views)
	{
		vector<MovePack> move_packs;
		for (auto& elem : views)
		{
			auto move_data = elem.PackMoveData();
			if (move_data.packs.size())
				move_packs.emplace_back(std::move(move_data));
		}
		if (move_packs.size())
		{
			connection_manager->CreateAndSendMessage<MoveClientMessage>(GameChannel::UNRELIABLE, [&](MoveClientMessage& msg)
				{
					msg.move_packs = std::move(move_packs);
				}
			);
		}
	}
}
