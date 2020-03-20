#include "stdafx.h"
#include "ClientMoveManager.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <network/SubstreamManager.inl>
#include <network/ConnectionManager.inl>
#include <network/MoveClientMessage.h>
#include <network/NetworkSystem.inl>
#include <network/GhostMessage.h>
#include <network/GhostFlags.h>
#include <network/IDManager.h>
#include <network/ElectronView.h>
#include <network/ack_utils.h>

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
		client.Subscribe<GhostMessage>([this](GhostMessage& msg) { OnGhostReceived(msg); });
		client.Subscribe<MoveAcknowledgementMessage>([this](MoveAcknowledgementMessage& msg) { OnMoveAcknowledgementReceived(msg); });
	}

	void ClientMoveManager::SubscribeEvents(ServerConnectionManager&)
	{
		IDK_ASSERT_MSG(false, "Do not attack clientmovemanager to server!");
	}

	void ClientMoveManager::SendMoves(span<ElectronView> views)
	{
		auto curr_seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
		vector<MovePack> move_packs;
		for (auto& elem : views)
		{
			auto move_data = elem.PackMoveData(curr_seq);
			if (move_data.packs.size())
			{
				LOG_TO(LogPool::NETWORK, "packing move for object %d", elem.network_id);
				move_packs.emplace_back(std::move(move_data));
			}
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

	void ClientMoveManager::OnGhostReceived(GhostMessage& ghost_msg)
	{
		auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
		for (auto& elem : ghost_msg.ghost_packs)
		{
			if (auto elec_view = id_man.GetViewFromId(elem.network_id))
				elec_view->UnpackGhostData(ghost_msg.sequence_number, elem);
		}

	}

	void ClientMoveManager::OnMoveAcknowledgementReceived(MoveAcknowledgementMessage& move_msg)
	{
		auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
		for (auto& elem : move_msg.objects)
		{
			if (auto eview = id_man.GetViewFromId(elem.network_id))
				eview->UnpackServerGuess(elem);
		}
	}
}
