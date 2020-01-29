#include "stdafx.h"
#include "GhostManager.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <network/GhostMessage.h>
#include <network/NetworkSystem.h>
#include <network/IDManager.h>
#include <network/ElectronTransformView.h>
#include <network/ClientConnectionManager.inl>
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

	void GhostManager::UpdateGhosts(span<ElectronTransformView> ghosts)
	{
		for (auto& ghost : ghosts)
			ghost.UpdateGhost();
	}

	void GhostManager::UpdateMasters(span<ElectronTransformView> ghosts)
	{
		for (auto& ghost : ghosts)
			ghost.UpdateMaster();
	}

	void GhostManager::SendGhosts(span<ElectronTransformView> views)
	{
		for (auto& ghost : views)
		{
			if (auto ghost_data = std::get_if<ElectronTransformView::PreviousFrame>(&ghost.ghost_data))
			{
				if (ghost_data->state_mask)
				{
					connection_manager->CreateAndSendMessage<GhostMessage>(GameChannel::RELIABLE, [&](GhostMessage& ghost_msg)
						{
							ghost_msg.network_id = ghost.GetNetworkID();
							ghost_msg.state_mask = ghost_data->state_mask;

							if (ghost_msg.state_mask & GhostFlags::TRANSFORM_POS)
								ghost_msg.AddPosition(ghost_data->position);

							if (ghost_msg.state_mask & GhostFlags::TRANSFORM_ROT)
								ghost_msg.AddRotation(ghost_data->rotation);

							if (ghost_msg.state_mask & GhostFlags::TRANSFORM_SCALE)
								ghost_msg.AddScale(ghost_data->scale);
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
			auto tfm_view = view->GetGameObject()->GetComponent<ElectronTransformView>();
			if (msg->state_mask)
			{
				if (tfm_view->interp_over_seconds != 0)
				{
					if (!(std::get_if<ElectronTransformView::GhostData>(&tfm_view->ghost_data) || std::get_if<void*>(&tfm_view->ghost_data)))
						return;

					auto ghost_data = ElectronTransformView::GhostData{};
					ghost_data.state_mask = msg->state_mask;
					if (tfm_view->sync_position)
					{
						if (auto pos = msg->GetPosition())
						{
							ghost_data.start_pos = tfm.position;
							ghost_data.end_pos = *pos;
						}
					}

					if (tfm_view->sync_rotation && msg->state_mask & GhostFlags::TRANSFORM_ROT)
					{
						if (auto rot = msg->GetRotation())
						{
							ghost_data.start_rot = tfm.rotation;
							ghost_data.end_rot = *rot;
						}
					}

					if (tfm_view->sync_scale && msg->state_mask & GhostFlags::TRANSFORM_SCALE)
					{
						ghost_data.start_scale = tfm.scale;
						ghost_data.end_scale = msg->scale;
					}
					ghost_data.t = 0;
					tfm_view->ghost_data = ghost_data;
				}
				else
				{
					if (msg->state_mask & GhostFlags::TRANSFORM_POS)
						tfm.position = msg->position;
					if (msg->state_mask & GhostFlags::TRANSFORM_ROT)
						tfm.rotation = msg->rotation;
					if (msg->state_mask & GhostFlags::TRANSFORM_SCALE)
						tfm.scale = msg->scale;
				}
			}
		}
	}

}