#include "stdafx.h"
#include "ElectronTransformView.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <network/ElectronView.inl>
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

	void ElectronTransformView::Start()
	{
		auto view = GetGameObject()->GetComponent<ElectronView>();
		if (!view)
			return;

		auto tfm = GetGameObject()->Transform();
		if (sync_position)
			view->RegisterMember<Transform, vec3>(tfm, &Transform::position, interp_over_seconds, 
				[dist= this->send_threshold * this->send_threshold](const vec3& lhs, const vec3& rhs)  ->bool
				{
					return (lhs - rhs).length_sq() > dist;
				});
		if (sync_rotation)
			view->RegisterMember(tfm, &Transform::rotation, 0.1);
		if (sync_scale)
			view->RegisterMember(tfm, &Transform::scale, 0.2);
	}
}
