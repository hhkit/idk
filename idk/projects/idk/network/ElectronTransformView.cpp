#include "stdafx.h"
#include "ElectronTransformView.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <network/ElectronView.h>

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
		if (auto prev_data = std::get_if<PreviousFrame>(&ghost_data))
		{
			PreviousFrame& previous_data = *prev_data;
			previous_data.state_mask = 0;

			auto& tfm = *GetGameObject()->Transform();
			if (sync_position)
			{
				const auto curr_pos = tfm.GlobalPosition();
				if ((curr_pos - previous_data.position).length_sq() > delta_threshold)
				{
					previous_data.position = curr_pos;
					previous_data.state_mask |= PreviousFrame::DIRTY_POS;
				}
			}

			if (sync_rotation)
			{
				const auto curr_rot = tfm.GlobalRotation();
				if (curr_rot != previous_data.rotation)
				{
					previous_data.rotation = curr_rot;
					previous_data.state_mask |= PreviousFrame::DIRTY_ROT;
				}
			}

			if (sync_scale)
			{
				const auto curr_scale = tfm.GlobalScale();
				if (curr_scale != previous_data.scale)
				{
					previous_data.scale = curr_scale;
					previous_data.state_mask |= PreviousFrame::DIRTY_SCALE;
				}
			}
			return true;
		}
		else 
			return false;
	}

	bool ElectronTransformView::UpdateGhost()
	{
		if (auto ghost_ = std::get_if<GhostData>(&ghost_data))
		{
			GhostData& ghost = *ghost_;
			auto advance = Core::GetDT() / interp_over;

			if (ghost.t >= 1)
				return true;

			ghost.t = std::min(1.f, ghost.t + advance);
			auto& tfm = *GetGameObject()->Transform();

			const auto t = ghost.t;

			if (sync_position)
				tfm.position = lerp(ghost.start_pos, ghost.end_pos, t);

			if (sync_rotation)
				tfm.rotation = slerp(ghost.start_rot, ghost.end_rot, t);

			if (sync_scale)
				tfm.scale = lerp(ghost.start_scale, ghost.end_scale, t);

			return true;
		}
		return false;
	}
}
