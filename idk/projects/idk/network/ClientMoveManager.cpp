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
#include <network/ControlObjectMessage.h>

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
		client.Subscribe<ControlObjectMessage>([this](ControlObjectMessage& msg) {this->OnControlObject(msg); });
	}

	void ClientMoveManager::SubscribeEvents(ServerConnectionManager&)
	{
		IDK_ASSERT_MSG(false, "Do not attack clientmovemanager to server!");
	}

	void ClientMoveManager::SendMoves(span<ElectronView> views)
	{
		const auto curr_seq = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
		vector<MovePack> move_packs;
		for (auto& elem : views)
		{
			if (std::get_if<ElectronView::ClientSideInputs>(&elem.move_state))
			{
				auto move_data = elem.PackMoveData(curr_seq);
				if (move_data.packs.size())
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

	void ClientMoveManager::OnControlObject(ControlObjectMessage& msg)
	{
		auto ev = Core::GetSystem<NetworkSystem>().GetIDManager().GetViewFromId(msg.network_id);

		if (ev)
		{
			if (auto val = std::get_if<ElectronView::ClientSideInputs>(&ev->move_state))
			{
				// ignore message if the control object is old
				if (val->last_received_control_object >= msg.move_ack)
					return;

				auto& move_buffer = val->moves;
				while (move_buffer.size() && move_buffer.front().index < msg.move_ack)
					move_buffer.pop_front();

				auto params = ev->GetParameters();
				for (unsigned i = 0; i < params.size(); ++i)
				{
					const auto& payload = msg.control_object_data[i];
					auto& param = params[i];

					auto& ghost = *param->GetGhost();
					ghost.UnpackData(msg.move_ack, payload);
				}

				val->last_received_control_object = msg.move_ack;
				val->dirty_control_object = true;
			}
		}
	}
}
