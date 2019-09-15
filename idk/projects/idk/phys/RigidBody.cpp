#include "stdafx.h"
#include "RigidBody.h"

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
	vec3 RigidBody::AddForce(const vec3& newtons)
	{
		auto added = newtons * inv_mass;
		accel += added;
		return added;
	}
	const mat4& RigidBody::PredictedTransform() const
	{
		return _predicted_tfm;
	}
}