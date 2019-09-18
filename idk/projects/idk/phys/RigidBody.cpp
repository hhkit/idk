#include "stdafx.h"
#include "RigidBody.h"
#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
	real RigidBody::mass() const
	{
		return 1 / inv_mass;
	}
	void RigidBody::mass(real new_mass)
	{
		inv_mass = 1 / new_mass;
	}
	vec3 RigidBody::velocity() const
	{
		return GetGameObject()->Transform()->GlobalPosition() - _prev_pos;
	}
	void RigidBody::velocity(const vec3& new_vel)
	{
		_prev_pos = GetGameObject()->Transform()->GlobalPosition() - new_vel;
	}
	bool RigidBody::sleeping() const
	{
		return _sleeping || sleep_next_frame;
	}
	void RigidBody::sleeping(bool new_bool)
	{
		_sleeping = new_bool;
	}
	vec3 RigidBody::AddForce(const vec3& newtons)
	{
		auto added = newtons * inv_mass;
		_accum_accel += added;
		return added;
	}
	const mat4& RigidBody::PredictedTransform() const
	{
		return _predicted_tfm;
	}
}