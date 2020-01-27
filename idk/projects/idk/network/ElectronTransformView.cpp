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

	void ElectronTransformView::UpdateGhost()
	{
		std::visit([&](auto& elem)
			{
				using T = std::decay_t<decltype(elem)>;
				if constexpr (std::is_same_v<T, PreviousFrame>)
				{
					PreviousFrame& previous_data = elem;
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
				}

				if constexpr (std::is_same_v<T, GhostData>)
				{
					GhostData& ghost = elem;
					auto advance = Core::GetDT() / interp_over;

					if (ghost.t >= 1)
						return;

					ghost.t = std::min(1.f, ghost.t + advance);
					auto& tfm = *GetGameObject()->Transform();

					const auto t = ghost.t;

					if (sync_position)
						tfm.GlobalPosition(lerp(ghost.start_pos, ghost.end_pos, t));

					if (sync_rotation)
						tfm.GlobalRotation(slerp(ghost.start_rot, ghost.end_rot, t));

					if (sync_scale)
						tfm.GlobalScale(lerp(ghost.start_scale, ghost.end_scale, t));
				}
			}, ghost_data);

	}
}
