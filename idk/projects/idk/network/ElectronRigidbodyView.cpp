#include "stdafx.h"
#include "ElectronRigidbodyView.h"
#include <core/GameObject.h>
#include <phys/RigidBody.h>
#include <network/ElectronView.inl>
#include <network/NetworkSystem.h>

namespace idk
{
	void ElectronRigidbodyView::Start()
	{
		auto view = GetGameObject()->GetComponent<ElectronView>();
		if (!view)
			return;

		auto rigidbody = GetGameObject()->GetComponent<RigidBody>();

		if (sync_velocity && rigidbody)
		{
			rb = rigidbody;

			ParameterImpl<vec3> param;
			param.getter = [rigidbody]() -> vec3 { return rigidbody->velocity(); };
			param.setter = [rigidbody](const vec3& v) -> void { rigidbody->velocity(v); };
			param.custom_move = [rigidbody](const vec3& v) -> void { LOG_TO(LogPool::NETWORK, "ADD FORCE"); rigidbody->AddForce(v); };
			velocity_param = view->RegisterMember("Velocity", std::move(param), 0);
		}
	}
	NetworkID ElectronRigidbodyView::GetNetworkID() const
	{
		return GetView()->network_id;
	}
	Handle<ElectronView> ElectronRigidbodyView::GetView() const
	{
		return GetGameObject()->GetComponent<ElectronView>();
	}
}
