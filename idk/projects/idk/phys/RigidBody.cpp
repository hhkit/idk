#include "stdafx.h"
#include "RigidBody.h"
#include <core/GameObject.inl>
#include <common/Transform.h>

namespace idk
{
	vec3 RigidBody::position() const
	{
		return GetGameObject()->Transform()->GlobalPosition();
	}
	void RigidBody::position(const vec3& new_pos)
	{
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
		return linear_velocity;
	}
	void RigidBody::velocity(const vec3& new_vel)
	{
		linear_velocity = new_vel;
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
		const auto added = newtons * inv_mass;
		force += added;
		return added;
	}
	vec3 RigidBody::AddTorque(const vec3& t)
	{
		torque += t;
	}
	void RigidBody::TeleportBy(const vec3& translation)
	{
		auto& tfm = *GetGameObject()->Transform();
		tfm.GlobalPosition(tfm.GlobalPosition() + translation);
	}
}