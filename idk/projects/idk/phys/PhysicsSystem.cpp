#include "stdafx.h"
#include "PhysicsSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <gfx/DebugRenderer.h>
#include <phys/RigidBody.h>
#include <phys/Collider.h>
#include <phys/collision_box.h>
#include <phys/collision_sphere.h>
#include <math/matrix_decomposition.h>

namespace idk
{
	void PhysicsSystem::ApplyGravity(span<class RigidBody> rbs)
	{
		for (auto& elem : rbs)
			if (elem.use_gravity && !elem.is_kinematic)
				elem.AddForce(vec3{ 0, -9.81, 0 });
	}
	void PhysicsSystem::PredictTransform(span<class RigidBody> rbs, span<const class Transform>)
	{
		const auto dt = Core::GetDT().count();
		const auto half_dt = dt / 2;
		
		for (auto& rigidbody : rbs)
		{
			auto tfm = rigidbody.GetGameObject()->Transform();

			// do this version if we are doing rotational as we have to rotate globally
			// auto decomp = decompose(tfm->GlobalMatrix());
			// decomp.position += rigidbody.velocity * dt;
			auto mat = tfm->GlobalMatrix();
			mat[3] += vec4{ rigidbody.velocity * dt + rigidbody.accel * dt * half_dt, 0 };

			rigidbody._predicted_tfm = mat;
		}
	}

	void PhysicsSystem::CollideObjects(span<class Collider> colliders, span<const class Transform>)
	{
		collisions.clear();

		const auto dt = Core::GetDT().count();

		constexpr auto debug_draw = [](const Collider& collider, const color& c = color{1,0,0})
		{
			std::visit([&](const auto& shape)
			{
				Core::GetSystem<DebugRenderer>().Draw(shape * collider.GetGameObject()->Transform()->GlobalMatrix(), c);
			}, collider.shape);
		};

		for (unsigned i = 0; i < colliders.size(); ++i)
		{
			for (unsigned j = i + 1; j < colliders.size(); ++j)
			{
				const auto& lcollider = colliders[i];
				const auto& rcollider = colliders[j];

				const auto collision = std::visit([&](const auto& lhs, const auto& rhs) -> phys::col_result
				{
					using LShape = std::decay_t<decltype(lhs)>;
					using RShape = std::decay_t<decltype(rhs)>;

					// put shape into world space
					constexpr auto calc_shape = [](const auto& shape, Handle<RigidBody> rb, const Collider& col)
					{
						if (rb)
							return shape * rb->PredictedTransform();
						else
							return shape * col.GetGameObject()->Transform()->GlobalMatrix();
					};

					// get rigidbodies
					const auto lrigidbody = lcollider.GetGameObject()->GetComponent<RigidBody>();
					const auto rrigidbody = rcollider.GetGameObject()->GetComponent<RigidBody>();

					// if both rbs are useless
					if (!lrigidbody && !rrigidbody)
						return phys::col_failure{};

					const auto lshape = calc_shape(lhs, lrigidbody, lcollider);
					const auto rshape = calc_shape(rhs, rrigidbody, rcollider);

					// static collisions
					if constexpr (std::is_same_v<LShape, box> && std::is_same_v<RShape, box>)
						return phys::collide_box_box_discrete(
							lshape, rshape);
					else
					if constexpr (std::is_same_v<LShape, sphere> && std::is_same_v<RShape, sphere>)
						return phys::collide_sphere_sphere_discrete(
							lshape, rshape);
					else
						return phys::col_failure{};

				}, lcollider.shape, rcollider.shape);

				if (collision)
				{
					debug_draw(lcollider, color{0,1,0});
					debug_draw(rcollider, color{0,1,1});
					collisions.emplace(CollisionPair{ lcollider.GetHandle(), rcollider.GetHandle() }, collision.success());
				}
				else
				{
					debug_draw(lcollider);
					debug_draw(rcollider);
				}
			}
		}

		for (auto& [pair, result] : collisions)
		{
			const auto& lcollider = pair.lhs;
			const auto& rcollider = pair.rhs;

			const auto lrigidbody = lcollider->GetGameObject()->GetComponent<RigidBody>();
			const auto rrigidbody = rcollider->GetGameObject()->GetComponent<RigidBody>();

			constexpr auto get_values =
				[](Handle<RigidBody> rb) -> std::tuple<vec3, real, real>
			{
				if (rb)
				{
					auto& ref = *rb;
					return std::make_tuple(ref.velocity, ref.restitution, ref.inv_mass);
				}
				else
				{
					return std::make_tuple(vec3{}, 1.f, 0.f);
				}
			};

			const auto [lvel, lrestitution, linv_mass] = get_values(lrigidbody);
			const auto [rvel, rrestitution, rinv_mass] = get_values(rrigidbody);

			auto rel_v = rvel - lvel; // a is not moving
			auto contact_v = rel_v.dot(result.normal_of_collision); // normal points towards A

			if (contact_v < +epsilon)
				continue;

			auto restitution = std::min(lrestitution, rrestitution);

			auto impulse_scalar = (1.f + restitution) * contact_v / (linv_mass + rinv_mass);
			auto impulse = impulse_scalar * result.normal_of_collision;

			if (lrigidbody)
				lrigidbody->velocity += impulse * linv_mass;
			if (rrigidbody)
				rrigidbody->velocity -= impulse * rinv_mass;
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
			tfm->GlobalPosition(
				tfm->GlobalPosition() + (rigidbody.velocity * dt) + rigidbody.accel * dt * half_dt);
			
			auto accel = rigidbody.accel;
			rigidbody.velocity += (accel + rigidbody._prev_accel) * half_dt;

			rigidbody._prev_accel = rigidbody.accel;
			rigidbody.accel       = vec3{};
		}
	}
	void PhysicsSystem::Init()
	{
	}
	void PhysicsSystem::Shutdown()
	{
	}

	size_t PhysicsSystem::pair_hasher::operator()(const CollisionPair& collision_pair) const
	{
		auto hash = std::hash<size_t>{}(collision_pair.lhs.id);
		hash_combine(hash, collision_pair.rhs.id);
		return hash;
	}
}