#include "stdafx.h"
#include "ElectronAnimatorView.h"
#include <core/GameObject.h>
#include <anim/Animator.h>
#include <network/ElectronView.inl>

namespace idk
{
	NetworkID ElectronAnimatorView::GetNetworkID() const
	{
		return GetView()->network_id;
	}
	Handle<ElectronView> ElectronAnimatorView::GetView() const
	{
		return GetGameObject()->GetComponent<ElectronView>();
	}
	void ElectronAnimatorView::Start()
	{
		// Get all synced params based on type and name. 
	}
}
