#include "stdafx.h"
#include "Collider.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <phys/RigidBody.h>

namespace idk
{
	bool Collider::is_enabled_and_active() const
	{
		return enabled && GetGameObject()->ActiveInHierarchy();
	}
	aabb Collider::bounds() const
	{
		return std::visit([&](const auto& shape) { return (shape * GetGameObject()->Transform()->GlobalMatrix()).bounds(); }, shape);
	}
	void Collider::setup_predict()
	{
		if (!_rigidbody)
		{
			auto search_go = GetGameObject();
			while (search_go && !_rigidbody)
			{
				_rigidbody = search_go->GetComponent<RigidBody>();
				search_go = search_go->Parent();
			}
		}

		_enabled_this_frame = is_enabled_and_active();
		_static = !s_cast<bool>(_rigidbody);
		if (_rigidbody)
		{
			_broad_phase = std::visit([&](const auto& shape) { return (shape * _rigidbody->PredictedTransform()).bounds(); }, shape);
			_broad_phase.grow(_rigidbody->velocity());
			_broad_phase.grow(-_rigidbody->velocity());
		}
		else
			_broad_phase = bounds();
	}
}
