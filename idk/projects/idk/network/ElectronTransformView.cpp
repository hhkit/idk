#include "stdafx.h"
#include "ElectronTransformView.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <phys/RigidBody.h>
#include <network/ElectronView.inl>
#include <network/GhostFlags.h>
#include <network/NetworkSystem.h>

namespace idk
{
	void ElectronTransformView::Start()
	{
		auto view = GetGameObject()->GetComponent<ElectronView>();
		if (!view)
			return;

		auto tfm = GetGameObject()->Transform();
		transform = tfm;
		if (sync_position)
		{
			ParameterImpl<vec3> param( tfm, &Transform::position);
			//param.predict_func = PredictionFunction::Quadratic;
			param.send_condition = 
				[dist = this->send_threshold * this->send_threshold](const vec3& lhs, const vec3& rhs) ->bool
			{
				return (lhs - rhs).length_sq() > dist;
			};
			pos_param = view->RegisterMember("Position", std::move(param), interp_over_seconds);
		}
		if (sync_rotation)
			rot_param = view->RegisterMember("Rotation", ParameterImpl<quat>(tfm, &Transform::rotation), 0.1f);
		if (sync_scale)
			scale_param = view->RegisterMember("Scale", ParameterImpl<vec3>(tfm, &Transform::scale), 0.2f);
	}

	NetworkID ElectronTransformView::GetNetworkID() const
	{
		return GetGameObject()->GetComponent<ElectronView>()->network_id;
	}

	Handle<ElectronView> ElectronTransformView::GetView() const
	{
		return GetGameObject()->GetComponent<ElectronView>();
	}
}
