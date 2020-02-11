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
		client.Subscribe<GhostMessage>([this](GhostMessage* msg) { OnGhostReceived(msg); });
		
		Core::GetSystem<NetworkSystem>().SubscribePacketResponse(&GhostManager::UpdateGhosts);
	}

	void GhostManager::SubscribeEvents(ServerConnectionManager& server)
	{
		// TODO: acknowledgment

		Core::GetSystem<NetworkSystem>().SubscribePacketResponse(&GhostManager::UpdateMasters); // note: will break on multiple connections
		OnFrameEnd(&GhostManager::SendGhosts);
	}

	void GhostManager::UpdateGhosts(span<ElectronView> ghosts)
	{
		for (auto& ghost : ghosts)
			ghost.UpdateGhost();
	}

	void GhostManager::UpdateMasters(span<ElectronView> ghosts)
	{
		for (auto& ghost : ghosts)
			ghost.UpdateMaster();
	}

	void GhostManager::SendGhosts(span<ElectronView> views)
	{
		for (auto& view : views)
		{
			if (view.owner != Host::SERVER)
				continue;

			if (auto ghost_state = std::get_if<ElectronView::Master>(&view.ghost_state))
			{
				if (view.state_mask)
				{
					LOG_TO(LogPool::NETWORK, "Sending Ghost Message for %d", view.network_id);
					connection_manager->CreateAndSendMessage<GhostMessage>(GameChannel::UNRELIABLE, [&](GhostMessage& ghost_msg)
						{
							ghost_msg.network_id = view.network_id;
							ghost_msg.sequence_number = Core::GetSystem<NetworkSystem>().GetSequenceNumber();
							ghost_msg.state_mask = view.state_mask;
							ghost_msg.pack = view.PackGhostData();
						});
				}
			}
		}
	}

	void GhostManager::OnGhostReceived(GhostMessage* msg)
	{
		auto& id_man = Core::GetSystem<NetworkSystem>().GetIDManager();
		auto view = id_man.GetViewFromId(msg->network_id);
		auto& tfm = *view->GetGameObject()->Transform();
		if (view)
		{
			// push the ghost data into the view
			LOG_TO(LogPool::NETWORK, "Received Ghost Message for %d", view->network_id);
			if (msg->state_mask)
			{
				if (!std::get_if<ElectronView::Ghost>(&view->ghost_state))
					return;
				if (std::get_if<ElectronView::ControlObject>(&view->move_state))
					return;

				view->ghost_state = ElectronView::Ghost{};
				view->state_mask = msg->state_mask;
				view->UnpackGhostData(msg->sequence_number, msg->pack);
			}
		}
	}

}