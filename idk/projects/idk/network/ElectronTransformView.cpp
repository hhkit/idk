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
			view->RegisterMember(tfm, &Transform::position);
		if (sync_rotation)
			view->RegisterMember(tfm, &Transform::rotation);
		if (sync_scale)
			view->RegisterMember(tfm, &Transform::scale);
	}
}
