#include "stdafx.h"
#include "ElectronRigidbodyView.h"
#include <core/GameObject.h>
#include <phys/RigidBody.h>
#include <network/ElectronView.inl>

namespace idk
{
	NetworkID ElectronRigidbodyView::GetNetworkID() const
	{
		return GetView()->network_id;
	}
	Handle<ElectronView> ElectronRigidbodyView::GetView() const
	{
		return GetGameObject()->GetComponent<ElectronView>();
	}
	void ElectronRigidbodyView::Start()
	{
		auto view = GetGameObject()->GetComponent<ElectronView>();
		if (!view)
			return;

		auto rb = GetGameObject()->GetComponent<RigidBody>();
		if (sync_velocity)
			view->RegisterMember<vec3>([rb]() -> vec3 { return rb->velocity(); },
				[rb](const vec3& v) -> void { rb->velocity(v);  },
				0);
	}
}
