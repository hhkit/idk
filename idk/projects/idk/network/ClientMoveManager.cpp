#include "stdafx.h"
#include "ClientMoveManager.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <network/SubstreamManager.inl>
#include <network/ConnectionManager.inl>
#include <network/MoveClientMessage.h>
#include <network/GhostFlags.h>
#include <network/ElectronTransformView.h>

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

	void ClientMoveManager::CachePreviousPositions(span<ElectronTransformView> views)
	{
	}

	void ClientMoveManager::SendMoves(span<ElectronTransformView> views)
	{
		using DeltaAndValue = ElectronTransformView::ClientObject::DeltaAndValue;
		for (auto& elem : views)
		{
			if (auto* move_data = std::get_if<ElectronTransformView::ClientObject>(&elem.network_data))
			{
				auto& tfm = *elem.GetGameObject()->Transform();
				DeltaAndValue dnv;
				if (elem.sync_position)
				{
					auto deltaPos = tfm.position - move_data->prev_position;
					if (deltaPos.length_sq() > elem.send_threshold* elem.send_threshold)
					{
						dnv.state_mask |= GhostFlags::TRANSFORM_POS;
						dnv.delPos = deltaPos;
						dnv.position = tfm.position;
					}
				}
				if (elem.sync_rotation)
				{
					auto deltaRot = tfm.rotation * move_data->prev_rotation.inverse();
					if (!quat_equals(deltaRot, quat{}))
					{
						dnv.state_mask |= GhostFlags::TRANSFORM_ROT;
						dnv.delRot = deltaRot;
						dnv.rotation = tfm.rotation;
					}
				}
				if (elem.sync_scale)
				{
					auto deltaScale = tfm.scale / move_data->prev_scale;
					if (tfm.scale != vec3{ 1 })
					{
						dnv.state_mask |= GhostFlags::TRANSFORM_SCALE;
						dnv.delScale = deltaScale;
						dnv.scale = tfm.scale;
					}
				}
				if (dnv.state_mask)
				{
					move_data->move_window.emplace_back(dnv);
					LOG_TO(LogPool::NETWORK, "Sending client move message for ID %d", elem.GetNetworkID());
					connection_manager->CreateAndSendMessage<MoveClientMessage>(GameChannel::FASTEST_GUARANTEED, [&](MoveClientMessage& msg)
						{
							msg.state_mask = dnv.state_mask;
							msg.network_id = elem.GetNetworkID();

							msg.scale = dnv.delScale;
							msg.rotation.x = dnv.delRot.x;
							msg.rotation.y = dnv.delRot.y;
							msg.rotation.z = dnv.delRot.z;
							msg.rotation.w = dnv.delRot.w;
							msg.translation = dnv.delPos;
						});
				}

				// finalize
				if (elem.sync_position) move_data->prev_position = elem.GetGameObject()->Transform()->position;
				if (elem.sync_rotation) move_data->prev_rotation = elem.GetGameObject()->Transform()->rotation;
				if (elem.sync_scale)    move_data->prev_scale = elem.GetGameObject()->Transform()->scale;
			}
		}
	}
}
