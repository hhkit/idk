#include "stdafx.h"
#include "PhysicsSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <gfx/DebugRenderer.h>
#include <phys/RigidBody.h>
#include <phys/Collider.h>
#include <phys/collision_sphere.h>

namespace idk
{
	void PhysicsSystem::CollideObjects(span<class Collider> colliders, span<const class Transform>)
	{
		constexpr auto debug_draw = [](const Collider& collider, const color& c = color{1,0,0})
		{
			std::visit([&](const auto& shape)
			{
				Core::GetSystem<DebugRenderer>().Draw(shape * collider.GetGameObject()->Transform()->GlobalMatrix(), c);
			}, collider.shape);
		};
		for (auto& elem : colliders)
		{
			//std::visit([&](const auto& shape)
			//{
			//	Core::GetSystem<DebugRenderer>().Draw(shape * elem.GetGameObject()->Transform()->GlobalMatrix(), color{1, 0.5, 0});
			//}, elem.shape);
		}

		for (unsigned i = 0; i < colliders.size(); ++i)
		{
			for (unsigned j = i + 1; j < colliders.size(); ++j)
			{
				const auto& lcollider = colliders[i];
				const auto& rcollider = colliders[j];
				// static collision
				const auto collision = std::visit([&](const auto& lhs, const auto& rhs) -> phys::col_result
				{
					using LShape = std::decay_t<decltype(lhs)>;
					using RShape = std::decay_t<decltype(rhs)>;

					if constexpr (std::is_same_v<LShape, sphere> && std::is_same_v<RShape, sphere>)
						return phys::collide_sphere_sphere_discrete(
							lhs * lcollider.GetGameObject()->Transform()->GlobalMatrix(),
							rhs * rcollider.GetGameObject()->Transform()->GlobalMatrix());

					return phys::col_failure{};
				}, lcollider.shape, rcollider.shape);

				if (collision)
				{
					debug_draw(lcollider, color{0,1,0});
					debug_draw(rcollider, color{0,1,0});
				}
				else
				{
					debug_draw(lcollider);
					debug_draw(rcollider);
				}
			}
		}
	}

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