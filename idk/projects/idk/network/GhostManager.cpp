#include "stdafx.h"
#include "GhostManager.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <res/ResourceHandle.inl>
#include <network/GhostMessage.h>
#include <network/NetworkSystem.inl>
#include <network/IDManager.h>
#include <network/ElectronTransformView.h>
#include <network/ConnectionManager.inl>
#include <network/SubstreamManager.inl>

namespace idk
{
	void GhostManager::SubscribeEvents(ClientConnectionManager& client)
	{
		client.Subscribe<GhostMessage>([this](GhostMessage& msg) { OnGhostReceived(msg); });
	}

	void GhostManager::SubscribeEvents(ServerConnectionManager& server)
	{
		// TODO: acknowledgment
	}

	void GhostManager::SendGhosts(Host target, span<ElectronView> views)
	{
		vector<GhostPack> ghost_packs;

		for (auto& view : views)
		{
			if (view.owner == target)
				continue;

			if (const auto ghost_state = std::get_if<ElectronView::Master>(&view.ghost_state))
			{
				auto retransmit_state_mask = 0;
				auto pack = view.MasterPackData(retransmit_state_mask);
				if (pack.data_packs.size())
					ghost_packs.emplace_back(std::move(pack));
			}
		}

		if (ghost_packs.size())
			connection_manager->CreateAndSendMessage<GhostMessage>(GameChannel::UNRELIABLE, [&](GhostMessage& msg)
				{
					msg.sequence_number = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
					msg.ghost_packs = std::move(ghost_packs);
				});
	}

	void GhostManager::OnGhostReceived(GhostMessage& msg)
	{
		auto seq = msg.sequence_number;

		for (auto& pack : msg.ghost_packs)
		{
			const auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
			
			if (const auto view = id_man.GetViewFromId(pack.network_id))
			{
				// push the ghost data into the view
				if (!std::get_if<ElectronView::Ghost>(&view->ghost_state))
					return;

				if (std::get_if<ElectronView::ClientObject>(&view->move_state))
					return;

				view->UnpackGhostData(seq, pack);
			}
		}
	}

}