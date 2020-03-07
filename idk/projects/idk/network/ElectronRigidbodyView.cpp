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
		if (sync_velocity && std::get_if<ElectronView::Ghost>(&view->ghost_state))
		{
			ParameterImpl<vec3> param;
			param.getter = [rb]() -> vec3 { return rb->velocity(); };
			param.setter = [rb](const vec3& v) -> void { rb->velocity(v);  };
			view->RegisterMember("Velocity", std::move(param), 0);
		}
	}
}
