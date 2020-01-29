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
			auto view = ghost.GetView();
			if (view->owner != Host::SERVER)
				continue;

			if (auto network_data = std::get_if<ElectronTransformView::PreviousFrame>(&ghost.network_data))
			{
				if (network_data->state_mask)
				{
					LOG_TO(LogPool::NETWORK, "Sending Ghost Message for %d", view->network_id);
					connection_manager->CreateAndSendMessage<GhostMessage>(GameChannel::RELIABLE, [&](GhostMessage& ghost_msg)
						{
							ghost_msg.network_id = ghost.GetNetworkID();
							ghost_msg.state_mask = network_data->state_mask;

							if (ghost_msg.state_mask & GhostFlags::TRANSFORM_POS)
								ghost_msg.AddPosition(network_data->position);

							if (ghost_msg.state_mask & GhostFlags::TRANSFORM_ROT)
								ghost_msg.AddRotation(network_data->rotation);

							if (ghost_msg.state_mask & GhostFlags::TRANSFORM_SCALE)
								ghost_msg.AddScale(network_data->scale);
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
			auto tfm_view = view->GetGameObject()->GetComponent<ElectronTransformView>();
			if (msg->state_mask)
			{
				if (tfm_view->interp_over_seconds != 0)
				{
					if (!(std::get_if<ElectronTransformView::GhostData>(&tfm_view->network_data) || std::get_if<void*>(&tfm_view->network_data)))
						return;

					auto network_data = ElectronTransformView::GhostData{};
					network_data.state_mask = msg->state_mask;
					if (tfm_view->sync_position)
					{
						if (auto pos = msg->GetPosition())
						{
							network_data.start_pos = tfm.position;
							network_data.end_pos = *pos;
						}
					}

					if (tfm_view->sync_rotation && msg->state_mask & GhostFlags::TRANSFORM_ROT)
					{
						if (auto rot = msg->GetRotation())
						{
							network_data.start_rot = tfm.rotation;
							network_data.end_rot = *rot;
						}
					}

					if (tfm_view->sync_scale && msg->state_mask & GhostFlags::TRANSFORM_SCALE)
					{
						network_data.start_scale = tfm.scale;
						network_data.end_scale = msg->scale;
					}
					network_data.t = 0;
					tfm_view->network_data = network_data;
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