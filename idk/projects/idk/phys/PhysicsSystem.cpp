#include "stdafx.h"
#include "PhysicsSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <gfx/DebugRenderer.h>
#include <phys/RigidBody.h>
#include <phys/Collider.h>
#include <phys/collision_detection/collision_box.h>
#include <phys/collision_detection/collision_sphere.h>
#include <phys/collision_detection/collision_box_sphere.h>
#include <math/matrix_decomposition.h>

namespace idk
{
	constexpr float restitution_slop = 0.01f;
	constexpr float penetration_min_slop = 0.001f;
	constexpr float penetration_max_slop = 0.9f;
	constexpr float damping = 0.99f;

	void PhysicsSystem::PhysicsTick(span<class RigidBody> rbs, span<class Collider> colliders, span<class Transform>)
	{
		// helper functions
		constexpr auto check_rb = [](Handle<RigidBody> h_rb) -> bool
		{
			if (h_rb)
				return !h_rb->sleeping();
			else
				return false;
		};
		// put shape into world space
		constexpr auto calc_shape = [](const auto& shape, Handle<RigidBody> rb, const Collider& col)
		{
			if (rb)
				return shape * rb->PredictedTransform();
			else
				return shape * col.GetGameObject()->Transform()->GlobalMatrix();
		};

		constexpr auto debug_draw = [calc_shape](const Collider& collider, const color& c = color{ 1,0,0 }, const seconds& dur = Core::GetDT())
		{
			std::visit([&](const auto& shape)
				{
					Core::GetSystem<DebugRenderer>().Draw(calc_shape(shape, collider.GetGameObject()->GetComponent<RigidBody>(), collider), c, dur);
				}, collider.shape);
		};


		// phases
		const auto ApplyGravity = [&]()
		{
			for (auto& elem : rbs)
			{
				if (elem.sleeping())
					continue;

				if (elem.use_gravity && !elem.is_kinematic)
					elem.AddForce(vec3{ 0, -9.81, 0 });
			}
		};

		const auto PredictTransform = [&]()
		{
			const auto dt = Core::GetDT().count();
			const auto half_dt = dt / 2;

			for (auto& rigidbody : rbs)
			{
				auto tfm = rigidbody.GetGameObject()->Transform();

				if (rigidbody.sleeping())
				{
					rigidbody._prev_pos = tfm->GlobalPosition() - rigidbody.initial_velocity * dt;
					rigidbody._predicted_tfm = tfm->GlobalMatrix();
					rigidbody.initial_velocity = vec3{};
					continue;
				};

				auto old_mat = tfm->GlobalMatrix();
				vec3 curr_pos = old_mat[3].xyz;

				// verlet integrate towards new position
				//auto new_pos = curr_pos + (curr_pos - rigidbody._prev_pos)*(damping) + rigidbody._accum_accel * dt * dt;
				auto new_pos = 2.f * curr_pos - rigidbody._prev_pos + rigidbody._accum_accel * dt * dt;
				rigidbody._accum_accel = vec3{};
				rigidbody._prev_pos = curr_pos;
				old_mat[3].xyz = new_pos;
				rigidbody._predicted_tfm = old_mat;
			}
		};

		const auto CollideObjects = [&]()
		{
			collisions.clear();

			const auto dt = Core::GetDT().count();
			for (auto& elem : colliders)
			{
				elem.setup_predict();
				//Core::GetSystem<DebugRenderer>().Draw(elem._broad_phase, color{ 1,1,0 });
			}

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


							// get rigidbodies
							const auto lrigidbody = lcollider._rigidbody;
							const auto rrigidbody = rcollider._rigidbody;

							// if both rbs are useless
							if (!check_rb(lrigidbody) && !check_rb(rrigidbody))
								return phys::col_failure{};

							if (!lcollider._broad_phase.overlaps(rcollider._broad_phase))
							{
								//Core::GetSystem<DebugRenderer>().Draw(lcollider._broad_phase, color{ 1,0,1 });
								//Core::GetSystem<DebugRenderer>().Draw(rcollider._broad_phase, color{ 1,0,1 });
								return phys::col_failure{};
							}

							const auto lshape = calc_shape(lhs, lrigidbody, lcollider);
							const auto rshape = calc_shape(rhs, rrigidbody, rcollider);

							// static collisions
							if constexpr (std::is_same_v<LShape, box> && std::is_same_v<RShape, box>)
								return phys::collide_box_box_discrete(
									lshape, rshape);
							else
							if constexpr (std::is_same_v<LShape, sphere> && std::is_same_v<RShape, box>)
								return -phys::collide_box_sphere_discrete(
									rshape, lshape);
							else
							if constexpr (std::is_same_v<LShape, sphere> && std::is_same_v<RShape, sphere>)
								return phys::collide_sphere_sphere_discrete(
									lshape, rshape);
							else
							if constexpr (std::is_same_v<LShape, box> && std::is_same_v<RShape, sphere>)
								return phys::collide_box_sphere_discrete(
									lshape, rshape);
							else
								return phys::col_failure{};

						}, lcollider.shape, rcollider.shape);

					if (collision)
					{
						//debug_draw(lcollider, color{ 0,1,0 }, seconds{ 0.5 });
						//debug_draw(rcollider, color{ 0,1,0 }, seconds{ 0.5 });
						collisions.emplace(CollisionPair{ lcollider.GetHandle(), rcollider.GetHandle() }, collision.value());
					}
					else
					{
						//debug_draw(lcollider, color{ 1,0, 0 });
						//debug_draw(rcollider, color{ 1,0, 0 });
					}
				}
			}

			for (auto& [pair, result] : collisions)
			{
				const auto& lcollider = pair.lhs;
				const auto& rcollider = pair.rhs;

				const auto lrigidbody = lcollider->_rigidbody;
				const auto rrigidbody = rcollider->_rigidbody;

				constexpr auto get_values =
					[](Handle<RigidBody> rb) -> std::tuple<vec3, real, real, RigidBody*>
				{
					if (rb)
					{
						auto& ref = *rb;
						return std::make_tuple(ref.PredictedTransform()[3].xyz - ref._prev_pos, ref.restitution, ref.inv_mass, &ref);
					}
					else
					{
						return std::make_tuple(vec3{}, 1.f, 0.f, nullptr);
					}
				};

				const auto [lvel, lrestitution, linv_mass, lrb_ptr] = get_values(lrigidbody);
				const auto [rvel, rrestitution, rinv_mass, rrb_ptr] = get_values(rrigidbody);

				auto rel_v = rvel - lvel; // a is not moving
				auto contact_v = rel_v.dot(result.normal_of_collision); // normal points towards A

				if (contact_v < +epsilon)
					continue;


				auto restitution = std::min(lrestitution, rrestitution);
				restitution = std::max(restitution - restitution_slop, 0.f);

				assert(result.penetration_depth > -epsilon);

				{
					const auto sum_inv_mass = linv_mass + rinv_mass;
					const auto impulse_scalar = (1.0f + restitution) * contact_v / sum_inv_mass;
					const auto impulse = damping * impulse_scalar * result.normal_of_collision;

					const auto penetration = std::max(result.penetration_depth - penetration_min_slop, 0.0f);
					const auto correction_vector = penetration * penetration_max_slop * result.normal_of_collision;

					if (lrb_ptr)
					{
						auto& ref_rb = *lrb_ptr;
						ref_rb._predicted_tfm[3].xyz = ref_rb._predicted_tfm[3].xyz + correction_vector;
						const auto new_vel = lvel + impulse * ref_rb.inv_mass;
						ref_rb._prev_pos = ref_rb._predicted_tfm[3].xyz - new_vel;
					}

					if (rrb_ptr)
					{
						auto& ref_rb = *rrb_ptr;
						ref_rb._predicted_tfm[3].xyz = ref_rb._predicted_tfm[3].xyz - correction_vector;
						const auto new_vel = rvel - impulse * ref_rb.inv_mass;
						ref_rb._prev_pos = ref_rb._predicted_tfm[3].xyz - new_vel;
					}
				}
			}
		};

		const auto FinalizePositions = [&]()
		{
			for (auto& rigidbody : rbs)
			{
				rigidbody.GetGameObject()->Transform()->GlobalMatrix(rigidbody._predicted_tfm);
				rigidbody.sleep_next_frame = false;
			}

			for (auto& collider : colliders)
				debug_draw(collider);
		};

		ApplyGravity();
		PredictTransform();
		for (int i = 0; i < 4;++i)
			CollideObjects();
		FinalizePositions();
	}

	void PhysicsSystem::DebugDrawColliders(span<class Collider> colliders)
	{
		// put shape into world space
		constexpr auto calc_shape = [](const auto& shape, Handle<RigidBody> rb, const Collider& col)
		{
			return shape * col.GetGameObject()->Transform()->GlobalMatrix();
		};

		constexpr auto debug_draw = [calc_shape](const Collider& collider, const color& c = color{ 1,0,0 }, const seconds& dur = Core::GetDT())
		{
			std::visit([&](const auto& shape)
				{
					Core::GetSystem<DebugRenderer>().Draw(calc_shape(shape, collider.GetGameObject()->GetComponent<RigidBody>(), collider), c, dur);
				}, collider.shape);
		};

		for (auto& collider : colliders)
			debug_draw(collider);
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