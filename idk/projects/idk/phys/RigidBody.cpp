#include "stdafx.h"
#include "RigidBody.h"
#include <core/GameObject.h>
#include <common/Transform.h>

namespace idk
{
	vec3 RigidBody::position() const
	{
		return GetGameObject()->Transform()->GlobalPosition();
	}
	void RigidBody::position(const vec3& new_pos)
	{
		auto curr_pos = GetGameObject()->Transform()->GlobalPosition();
		auto vel = curr_pos - _prev_pos;
		_prev_pos = new_pos - vel;
		GetGameObject()->Transform()->GlobalPosition(new_pos);
	}
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
		_prev_pos = GetGameObject()->Transform()->GlobalPosition() - new_vel / Core::GetDT().count();
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
	void RigidBody::TeleportBy(const vec3& translation)
	{
		auto& tfm = *GetGameObject()->Transform();

		_prev_pos += translation;
		tfm.GlobalPosition(tfm.GlobalPosition() + translation);
	}
}