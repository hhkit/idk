#include "stdafx.h"
#include "RigidBody.h"

namespace idk
{
	vec3 RigidBody::AddForce(const vec3& newtons)
	{
		auto added = newtons / mass;
		accel += added;
		return added;
	}
}