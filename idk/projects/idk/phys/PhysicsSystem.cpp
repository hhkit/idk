#include "stdafx.h"
#include "PhysicsSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <phys/RigidBody.h>

namespace idk
{
	void PhysicsSystem::MoveObjects(span<RigidBody> rbs, span<Transform> /* we will be writing to transforms */)
	{
		auto dt = Core::GetDT().count();
		auto half_dt = dt / 2;

		for (auto& rigidbody : rbs)
		{
			auto tfm = rigidbody.GetGameObject()->Transform();

			// velocity verlet
			auto accel = rigidbody.accel;
			auto old_v = rigidbody.velocity;
			rigidbody.velocity += accel * dt;

			tfm->GlobalPosition(tfm->GlobalPosition() + (rigidbody.velocity + old_v) * half_dt);
			rigidbody.accel = vec3{};
		}
	}
	void PhysicsSystem::Init()
	{
	}
	void PhysicsSystem::Shutdown()
	{
	}
}