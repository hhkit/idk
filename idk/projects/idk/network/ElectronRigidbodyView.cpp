#include "stdafx.h"
#include "ElectronRigidbodyView.h"
#include <core/GameObject.h>
#include <phys/RigidBody.h>
#include <network/ElectronView.inl>
#include <network/NetworkSystem.h>

namespace idk
{
	void ElectronRigidbodyView::SetVelocity(const vec3& v)
	{
		if (velocity_param)
			velocity_param->GetClientObject()->PushMove(
				Core::GetSystem<NetworkSystem>().GetSequenceNumber(), 
				SeqAndPack::set_move, 
				v
			);

	}

	void ElectronRigidbodyView::AddForce(const vec3& force)
	{
		if (velocity_param)
		{
			velocity_param->GetClientObject()->PushMove(
				Core::GetSystem<NetworkSystem>().GetSequenceNumber(),
				SeqAndPack::delta_move,
				force * rb->inv_mass * Core::GetDT().count()
			);
		}
	}

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
