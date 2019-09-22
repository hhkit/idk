#include "stdafx.h"
#include "Collider.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <phys/RigidBody.h>

namespace idk
{
	aabb Collider::bounds() const
	{
		return std::visit([&](const auto& shape) { return (shape * GetGameObject()->Transform()->GlobalMatrix()).bounds(); }, shape);
	}
	void Collider::setup_predict()
	{
		_rigidbody = GetGameObject()->GetComponent<RigidBody>();

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
