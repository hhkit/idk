#include "stdafx.h"
#include "ElectronTransformView.h"
#include <core/GameObject.inl>
#include <network/ElectronView.h>

namespace idk
{
	NetworkID ElectronTransformView::GetNetworkID() const
	{
		return GetGameObject()->GetComponent<ElectronView>()->network_id;
	}
}
