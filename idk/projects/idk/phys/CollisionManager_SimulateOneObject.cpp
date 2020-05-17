#include "stdafx.h"

#include <cmath>

#include <core/GameObject.inl>
#include <common/Transform.h>
#include <common/Layer.h>

#include <math/matrix_decomposition.h>
#include <math/matrix_transforms.h>

#include <phys/PhysicsSystem.h>
#include <phys/CollisionManager.h>
#include <phys/ContactSolver.h>
#include <phys/Collider.h>
#include <phys/RigidBody.h>
#include <phys/Collision_utils.h>
#include <phys/raycasts/collision_raycast.h>
#include <phys/collision_detection/collision_box.h>
#include <phys/collision_detection/collision_sphere.h>
#include <phys/collision_detection/collision_box_sphere.h>
#include <phys/collision_detection/collision_capsule.h>
#include <phys/collision_detection/collision_capsule_box.h>
#include <phys/collision_detection/collision_capsule_sphere.h>
#include <phys/collision_detection/ManagedCollision.h>
#include <phys/collision_contact.h>

#include <gfx/DebugRenderer.h>

#include <scene/SceneManager.h>
#include <scene/Scene.h>

#include <ds/result.inl>

namespace idk
{

	void CollisionManager::SimulateOneObject(Handle<RigidBody> rb)
	{
		const auto dt = Core::GetDT().count();

		// apply gravity and forces
		auto& rigidbody = *rb;
		const auto tfm = rigidbody.GetGameObject()->Transform();
		rigidbody._global_cache = tfm->GlobalMatrix();
		rigidbody._rotate_cache = tfm->GlobalRotation();

		if (rigidbody.sleeping())
		{
			rigidbody._prev_pos = rigidbody._global_cache[3].xyz;
			rigidbody.linear_velocity = rigidbody.initial_velocity * dt;
			rigidbody.initial_velocity = vec3{};
		}
		else
		{
			// Apply gravity
			if (rigidbody.use_gravity && !rigidbody.is_kinematic)
				rigidbody.AddForce(vec3{ 0, -9.81f * rigidbody.gravity_scale * rigidbody.mass(), 0 });

			// Apply forces
			if (!rigidbody.is_kinematic)
			{
				rigidbody.linear_velocity += (rigidbody.force * rigidbody.inv_mass) * dt;
				rigidbody.linear_velocity *= 1.0f / (1.0f + dt * rigidbody.linear_damping);

				// Compute the angular stuff here too. 
				if (!rigidbody.freeze_rotation)
					rigidbody.angular_velocity *= 1.0f / (1.0f + dt * rigidbody.angular_damping);

				// LOG_TO(LogPool::PHYS, "Force: (%f, %f, %f) ||  Velocity: (%f, %f, %f)", 
				//		  rigidbody.force.x, rigidbody.force.y, rigidbody.force.z, rigidbody.linear_velocity.x, rigidbody.linear_velocity.y, rigidbody.linear_velocity.z);
			}
			else
			{
				rigidbody.linear_velocity = (rigidbody._global_cache[3].xyz - rigidbody._prev_pos) / dt;
				rigidbody.angular_velocity = vec3{ 0.0f };
			}

			// Clear all forces
			rigidbody.force = vec3{ 0.0f };
			rigidbody.torque = vec3{ 0.0f };
		}

		vector<ColliderInfo> collidees;
		auto collider_creator = [&](const auto& shape) 
		{ 
			auto pred_shape = shape * rb->_global_cache; 
			auto& rb = rigidbody;
			
			// Compute the inverse world inertia tensor
			using Shape = decltype(pred_shape);

			const mat3 curr_rot = quat_cast<mat3>(rb._rotate_cache);
			const mat3 curr_rot_inv = curr_rot.transpose();

			if constexpr (std::is_same_v<Shape, box>)
			{
				constexpr float scalar = 1.0f / 12.0f;

				const vec3 extents = pred_shape.half_extents();
				const vec3 extents_sq = extents * extents;

				const float mass = rb.mass();
				const float ex2 = 4.0f * extents.x * extents.x;
				const float ey2 = 4.0f * extents.y * extents.y;
				const float ez2 = 4.0f * extents.z * extents.z;
				const float x = scalar * mass * (ey2 + ez2);
				const float y = scalar * mass * (ex2 + ez2);
				const float z = scalar * mass * (ex2 + ey2);

				const mat3 local_tensor{
					vec3{1.0f / x,   0.0f, 0.0f},
					vec3{0.0f, 1.0f / y,   0.0f},
					vec3{0.0f, 0.0f, 1.0f / z }
				};

				rb._global_inertia_tensor = curr_rot * (rb.inv_mass * local_tensor) * curr_rot_inv;
			}
			else if constexpr (std::is_same_v<Shape, sphere>)
			{
				// 1/mass * (0.4 * r^2) = 1 / (mass * 0.4 * r^2)
				const float diag_val = (1.0f / (0.4f * pred_shape.radius * pred_shape.radius));
				auto gg = vec3{ diag_val };
				const mat3 local_tensor = mat3{ scale(gg) };
				rb._global_inertia_tensor = curr_rot * (rb.inv_mass * local_tensor) * curr_rot_inv;
			}
			else // Only sphere and box has rotational
			{
				rb.freeze_rotation = true;
			}

			ColliderInfo one_obj_info;
			one_obj_info.rb = &rigidbody;
			one_obj_info.broad_phase = pred_shape.bounds();
			one_obj_info.layer = rigidbody.GetGameObject()->Layer();
			one_obj_info.predicted_shape = pred_shape;
			return one_obj_info;
		};

		vector<Handle<Collider>> cols;
		// assemble all colliders in tree
		auto sg = Core::GetSystem<SceneManager>().FetchSceneGraphFor(rb->GetGameObject());
		sg.Visit([&](Handle<GameObject> elem, int)
		{
			for (auto& col : elem->GetComponents<Collider>())
				cols.emplace_back(col);
		});

		for (auto& elem : cols)
		{
			auto& val = collidees.emplace_back(std::visit(collider_creator, elem->shape));
			val.collider = &*elem;
		}

		// test collisions
		auto& phys = Core::GetSystem<PhysicsSystem>();
		_info.clear();
		_info.reserve(_dynamic_info.size() * 4);

		// Dynamic vs Dynamic Broadphase
		for (auto& left_elem : collidees)
		{
			const auto& lrigidbody = rigidbody;
			const bool lrb_sleeping = lrigidbody.sleeping();
			for (auto& right_elem : _dynamic_info)
			{
				const auto& rrigidbody = *right_elem.rb;

				if (lrb_sleeping && rrigidbody.sleeping())
					continue;
				if (lrigidbody.GetHandle() == rrigidbody.GetHandle())
					continue;
				if (!phys.AreLayersCollidable(left_elem.layer, right_elem.layer))
					continue;
				if (!left_elem.broad_phase.overlaps(right_elem.broad_phase))
					continue;

				_info.emplace_back(ColliderInfoPair{ &left_elem, &right_elem });
			}

			// Static vs Dynamic Broadphase
			_static_broadphase.query_collisions(left_elem, _info);
		}

		// Narrow phase.
		auto new_collisions = CollisionList{};
		new_collisions.reserve(_info.size());

		constraint_states.clear();
		constraint_states.reserve(_info.size());
		for (auto& pair : _info)
		{
			auto& i = pair.lhs;
			auto& j = pair.rhs;
			const auto collision = CollideShapes(i->predicted_shape, j->predicted_shape);
			if (collision)
			{
				const auto col_val = collision.value();
				ContactConstraintState ccs;
				// Both must not be triggers.
				ccs.solve = !i->collider->is_trigger && !j->collider->is_trigger;
				if (ccs.solve)
				{
					// broadphase center and world center are the same
					ccs.centerA = col_val.centerA;
					ccs.centerB = col_val.centerB;
					ccs.rbA = i->rb;
					ccs.rbB = j->rb;
					ccs.mA = i->rb->inv_mass;
					ccs.mB = j->rb ? j->rb->inv_mass : 0.0f;
					ccs.iA = ccs.rbA->_global_inertia_tensor;
					ccs.iB = ccs.rbB ? ccs.rbB->_global_inertia_tensor : mat3{ vec3{0.0f}, vec3{0.0f}, vec3{0.0f} };
					ccs.restitution = min(i->collider->bounciness, j->collider->bounciness);
					ccs.static_friction = std::sqrt(i->collider->static_friction * j->collider->static_friction);
					ccs.friction = std::sqrt(i->collider->dynamic_friction * j->collider->dynamic_friction);

					ccs.tangentVectors[0] = col_val.tangentVectors[0];
					ccs.tangentVectors[1] = col_val.tangentVectors[1];
					ccs.normal = col_val.normal;

					ccs.contactCount = col_val.contactCount;
					for (int k = 0; k < ccs.contactCount; ++k)
					{
						ContactState* s = ccs.contacts + k;
						const phys::ContactPoint* cp = col_val.contacts + k;

						s->ra = cp->position - ccs.centerA;
						s->rb = cp->position - ccs.centerB;
						s->penetration = cp->penetration;
					}

					constraint_states.emplace_back(ccs);
				}
				new_collisions.emplace(CollisionPair{ i->collider->GetHandle(), j->collider->GetHandle() }, col_val);
			}
		}

		PreSolve();
		Solve();

		// finalize
		{
			if (!rigidbody.is_kinematic)
			{
				// Velocity
				vec3 translate = rigidbody._global_cache[3].xyz;
				if (rigidbody.linear_velocity.dot(rigidbody.linear_velocity) > 0.0001f)
				{
					translate = translate + rigidbody.linear_velocity * dt;
				}
				else
				{
					rigidbody.linear_velocity = vec3{ 0.0f };
				}
				// Rotational
				// Only do if there is angular velocity and rotational is not frozen
				if (!rigidbody.freeze_rotation && rigidbody.angular_velocity.dot(rigidbody.angular_velocity) > 0.0001f)
				{
					// Scale
					const vec3 s = [](const mat4& mat)
					{
						auto det = mat.determinant();
						return vec3(det < -epsilon ? -mat[0].length() : mat[0].length(), mat[1].length(), mat[2].length());
					}(rigidbody._global_cache);

					// Rotation
					quat q = rigidbody._rotate_cache;
					q = q.integrate(rigidbody.angular_velocity * dt);
					const mat4 r = quat_cast<mat4>(q);

					rigidbody._global_cache = r * scale(s);
				}
				else
				{
					rigidbody.angular_velocity = vec3{ 0.0f };
				}

				rigidbody._global_cache[3].xyz = translate;
				rigidbody.GetGameObject()->Transform()->GlobalMatrix(rigidbody._global_cache);
			}
			else
			{
				rigidbody.linear_velocity = (rigidbody._global_cache[3].xyz - rigidbody._prev_pos) / dt;
				rigidbody._prev_pos = rigidbody._global_cache[3].xyz;
				rigidbody.angular_velocity = vec3{ 0.0f };
			}

			rigidbody.sleep_next_frame = false;
			// rigidbody._done_this_frame = true;
		}

		static vector<CollisionList::value_type> old_objs;
		old_objs.clear();

		for (auto& elem : _collisions)
		{
			if (elem.first.lhs == rb || elem.first.rhs == rb)
			{
				if (const auto itr = new_collisions.find(elem.first);
					itr == new_collisions.end())
					old_objs.emplace_back(elem); // objects no longer colliding
			}
		}

		for (auto& elem : new_collisions)
			_collisions[elem.first] = elem.second;

		for (auto& elem : old_objs)
			_collisions.erase(elem.first);
	}
}