#include "stdafx.h"
#include "ElectronTransformView.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <network/ElectronView.h>
#include <network/GhostFlags.h>

namespace idk
{
	NetworkID ElectronTransformView::GetNetworkID() const
	{
		return GetGameObject()->GetComponent<ElectronView>()->network_id;
	}

	Handle<ElectronView> ElectronTransformView::GetView() const
	{
		return GetGameObject()->GetComponent<ElectronView>();
	}

	bool ElectronTransformView::UpdateMaster()
	{
		if (auto prev_data = std::get_if<PreviousFrame>(&network_data))
		{
			PreviousFrame& previous_data = *prev_data;
			previous_data.state_mask = 0;

			auto& tfm = *GetGameObject()->Transform();
			if (sync_position)
			{
				const auto curr_pos = tfm.position;
				if ((curr_pos - previous_data.position).length_sq() > send_threshold * send_threshold)
				{
					previous_data.position = curr_pos;
					previous_data.state_mask |= GhostFlags::TRANSFORM_POS;
				}
			}

			if (sync_rotation)
			{
				const auto curr_rot = tfm.rotation;
				if (curr_rot != previous_data.rotation)
				{
					previous_data.rotation = curr_rot;
					previous_data.state_mask |= GhostFlags::TRANSFORM_ROT;
				}
			}

			if (sync_scale)
			{
				const auto curr_scale = tfm.scale;
				if (curr_scale != previous_data.scale)
				{
					previous_data.scale = curr_scale;
					previous_data.state_mask |= GhostFlags::TRANSFORM_SCALE;
				}
			}
			return true;
		}
		else 
			return false;
	}

	bool ElectronTransformView::UpdateGhost()
	{
		if (auto ghost_ = std::get_if<GhostData>(&network_data))
		{
			GhostData& ghost = *ghost_;
			auto advance = Core::GetDT().count() / interp_over_seconds;

			if (ghost.t >= 1)
				return true;

			auto& tfm = *GetGameObject()->Transform();
			// snap
			if ((ghost.end_pos - tfm.position).length_sq() > snap_threshold * snap_threshold)
				ghost.t = 1;

			ghost.t = std::min(1.f, ghost.t + advance);

			const auto t = ghost.t;

			if (sync_position && (ghost.state_mask & GhostFlags::TRANSFORM_POS))
				tfm.position = lerp(ghost.start_pos, ghost.end_pos, t);

			if (sync_rotation && (ghost.state_mask & GhostFlags::TRANSFORM_ROT))
				tfm.rotation = slerp(ghost.start_rot, ghost.end_rot, t);

			if (sync_scale && (ghost.state_mask & GhostFlags::TRANSFORM_SCALE))
				tfm.scale = lerp(ghost.start_scale, ghost.end_scale, t);

			return true;
		}
		return false;
	}
}
