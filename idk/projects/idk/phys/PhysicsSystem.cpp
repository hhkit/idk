#include "stdafx.h"
#include "PhysicsSystem.h"
#include <core/GameObject.h>
#include <common/Transform.h>
#include <common/Layer.h>
#include <gfx/DebugRenderer.h>
#include <phys/RigidBody.h>
#include <phys/Collider.h>
#include <phys/collision_detection/collision_box.h>
#include <phys/collision_detection/collision_sphere.h>
#include <phys/collision_detection/collision_box_sphere.h>
#include <phys/collision_detection/collision_capsule.h>
#include <phys/collision_detection/ManagedCollision.h>

#include <script/MonoBehavior.h>
#include <script/MonoFunctionInvoker.h>

#include <math/matrix_decomposition.h>

namespace idk
{
	constexpr float restitution_slop = 0.01f;
	constexpr float penetration_min_slop = 0.001f;
	constexpr float penetration_max_slop = 0.5f;
	constexpr float damping = 0.99f;

	constexpr auto calc_shape = [](const auto& shape, const Collider& col)
	{
		return shape * col.GetGameObject()->Transform()->GlobalMatrix();
	};

	void PhysicsSystem::PhysicsTick(span<class RigidBody> rbs, span<class Collider> colliders, span<class Transform>)
	{
        for (auto& elem : colliders)
        {
            elem.find_rigidbody();
            if (elem._static_cache)
                elem.setup_predict();
			elem._enabled_this_frame = elem.is_enabled_and_active() && elem.GetHandle().scene != Scene::prefab;
		}

		Core::GetGameState().SortObjectsOfType<Collider>([](const Collider& lhs, const Collider& rhs)
		{
            return lhs._static_cache < rhs._static_cache;
		});

		// helper functions
		constexpr auto check_rb = [](Handle<RigidBody> h_rb) -> bool
		{
			if (h_rb)
				return !h_rb->sleeping() && h_rb.scene != Scene::prefab;
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
					Core::GetSystem<DebugRenderer>().Draw(calc_shape(shape, collider.GetGameObject()->GetComponent<RigidBody>(), collider), 
						collider.is_enabled_and_active() ? collider.is_trigger ? color{ 0, 1, 1 } : c : color{0.5}, dur);
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
				const auto tfm = rigidbody.GetGameObject()->Transform();

				if (rigidbody.sleeping())
				{
					rigidbody._prev_pos = tfm->GlobalPosition() - rigidbody.initial_velocity * dt;
					rigidbody._predicted_tfm = tfm->GlobalMatrix();
					rigidbody.initial_velocity = vec3{};
					continue;
				};

				if (!rigidbody.is_kinematic)
				{
					auto old_mat = tfm->GlobalMatrix();
					const vec3 curr_pos = old_mat[3].xyz;

					// verlet integrate towards new position
					//auto new_pos = curr_pos + (curr_pos - rigidbody._prev_pos)*(damping) + rigidbody._accum_accel * dt * dt;
					auto new_pos = 2.f * curr_pos - rigidbody._prev_pos + rigidbody._accum_accel * dt * dt;
					rigidbody._accum_accel = vec3{};
					rigidbody._prev_pos = curr_pos;
					old_mat[3].xyz = new_pos;
					rigidbody._predicted_tfm = old_mat;
				}
				else
				{
					rigidbody._predicted_tfm = tfm->GlobalMatrix();
				}
			}
		};

		const auto CollideObjects = [&]()
		{
			CollisionList collisionframe;

			const auto dt = Core::GetDT().count();

            for (auto& elem : colliders)
            {
                if (!elem._static_cache)
                    elem.setup_predict();
            }


			for (unsigned i = 0; i < colliders.size(); ++i)
			{
				const auto& lcollider = colliders[i];
				if (!lcollider._enabled_this_frame)
					continue;

				if (lcollider._static_cache)
					break;

				for (unsigned j = i + 1; j < colliders.size(); ++j)
				{
					const auto& rcollider = colliders[j];

					if (!rcollider._enabled_this_frame)
						continue;

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

							if (lcollider.GetGameObject() == rcollider.GetGameObject())
								return phys::col_failure{};

							if (!lcollider._broad_phase.overlaps(rcollider._broad_phase))
							{
								//Core::GetSystem<DebugRenderer>().Draw(lcollider._broad_phase, color{ 1,0,1 });
								//Core::GetSystem<DebugRenderer>().Draw(rcollider._broad_phase, color{ 1,0,1 });
								return phys::col_failure{};
							}

							const auto lshape = lhs; //calc_shape(lhs, lrigidbody, lcollider);
							const auto rshape = rhs; //calc_shape(rhs, rrigidbody, rcollider);

							// static collisions
							if constexpr (std::is_same_v<LShape, box>&& std::is_same_v<RShape, box>)
								return phys::collide_box_box_discrete(
									lshape, rshape);
							else
							if constexpr (std::is_same_v<LShape, sphere>&& std::is_same_v<RShape, box>)
								return -phys::collide_box_sphere_discrete(
									rshape, lshape);
							else
							if constexpr (std::is_same_v<LShape, sphere>&& std::is_same_v<RShape, sphere>)
								return phys::collide_sphere_sphere_discrete(
										lshape, rshape);
							else
							if constexpr (std::is_same_v<LShape, box>&& std::is_same_v<RShape, sphere>)
								return phys::collide_box_sphere_discrete(
											lshape, rshape);
							else
								if constexpr (std::is_same_v<LShape, capsule>&& std::is_same_v<RShape, sphere>)
									return phys::collide_capsule_sphere_discrete(
										lshape, rshape);
							else
							if constexpr (std::is_same_v<LShape, sphere>&& std::is_same_v<RShape, capsule>)
								return phys::collide_capsule_sphere_discrete(
									rshape, lshape);
							else
								return phys::col_failure{};

						}, lcollider._predicted_shape, rcollider._predicted_shape);

					if (collision)
					{
						//debug_draw(lcollider, color{ 0,1,0 }, seconds{ 0.5 });
						//debug_draw(rcollider, color{ 0,1,0 }, seconds{ 0.5 });
						collisionframe.emplace(CollisionPair{ lcollider.GetHandle(), rcollider.GetHandle() }, collision.value());
					}
					else
					{
						//debug_draw(lcollider, color{ 1,0, 0 });
						//debug_draw(rcollider, color{ 1,0, 0 });
					}
				}
			}

			for (const auto& [pair, result] : collisionframe)
			{
				const auto& lcollider = *pair.lhs;
				const auto& rcollider = *pair.rhs;

				const auto lrigidbody = lcollider._rigidbody;
				const auto rrigidbody = rcollider._rigidbody;

				// triggers do not require resolution
				if (lcollider.is_trigger || rcollider.is_trigger)
					continue;

				struct RigidBodyInfo
				{
					vec3 velocity = {};
					real inv_mass = 0.f;
					RigidBody* ref = nullptr;
				};

				constexpr auto get_values =
					[](Handle<RigidBody> rb) -> RigidBodyInfo
				{
					if (rb)
					{
						auto& ref = *rb;
						return { ref.PredictedTransform()[3].xyz - ref._prev_pos, ref.inv_mass, &ref };
					}
					else
					{
						return RigidBodyInfo{};
					}
				};

				const auto [lvel, linv_mass, lrb_ptr] = get_values(lrigidbody);
				const auto [rvel, rinv_mass, rrb_ptr] = get_values(rrigidbody);

				auto rel_v = rvel - lvel; // a is not moving
				auto contact_v = rel_v.dot(result.normal_of_collision); // normal points towards A

				if (contact_v < +epsilon)
					continue;

				// determine collision distribution
				auto restitution = (lcollider.bounciness, rcollider.bounciness) * .5f;
				restitution = std::max(restitution - restitution_slop, 0.f);
				IDK_ASSERT(result.penetration_depth > -epsilon);

				// determine friction disribution

				{
					const auto sum_inv_mass = linv_mass + rinv_mass;
					const auto collision_impulse_scalar = (1.0f + restitution) * contact_v / sum_inv_mass;
					const auto collision_impulse = damping * collision_impulse_scalar * result.normal_of_collision;

					const auto penetration = std::max(result.penetration_depth - penetration_min_slop, 0.0f);
					const auto correction_vector = penetration * penetration_max_slop * result.normal_of_collision;

					const auto tangent = (rel_v - (rel_v.dot(result.normal_of_collision)) * result.normal_of_collision).normalize();
					const auto frictional_impulse_scalar = (1.0f + restitution) * rel_v.dot(tangent) / sum_inv_mass;
					const auto mu = (lcollider.static_friction + rcollider.static_friction) * .5f;
					const auto jtangential = -rel_v.dot(tangent) / sum_inv_mass;

					const auto frictional_impulse = abs(jtangential) < frictional_impulse_scalar * mu
						? frictional_impulse_scalar * tangent
						: (lcollider.dynamic_friction, rcollider.dynamic_friction) * .5f * frictional_impulse_scalar * tangent;

					if (lrb_ptr && !lrb_ptr->is_kinematic)
					{
						auto& ref_rb = *lrb_ptr;
						ref_rb._predicted_tfm[3].xyz = ref_rb._predicted_tfm[3].xyz + correction_vector;
						const auto new_vel = lvel + (collision_impulse + frictional_impulse) * ref_rb.inv_mass;
						ref_rb._prev_pos = ref_rb._predicted_tfm[3].xyz - new_vel;
					}

					if (rrb_ptr && !rrb_ptr->is_kinematic)
					{
						auto& ref_rb = *rrb_ptr;
						ref_rb._predicted_tfm[3].xyz = ref_rb._predicted_tfm[3].xyz - correction_vector;
						const auto new_vel = rvel - (collision_impulse + frictional_impulse) * ref_rb.inv_mass;
						ref_rb._prev_pos = ref_rb._predicted_tfm[3].xyz - new_vel;
					}
				}
			}

			collisions.merge(collisionframe);
		};

		const auto FinalizePositions = [&]()
		{
			for (auto& rigidbody : rbs)
			{
				if (!rigidbody.is_kinematic)
					rigidbody.GetGameObject()->Transform()->GlobalMatrix(rigidbody._predicted_tfm);
				else
					rigidbody._prev_pos = rigidbody.GetGameObject()->Transform()->GlobalPosition();

				rigidbody.sleep_next_frame = false;
			}

			for (auto& collider : colliders)
				debug_draw(collider);
		};

		collisions.clear();

		ApplyGravity();
		PredictTransform();
		for (int i = 0; i < 4;++i)
			CollideObjects();
		FinalizePositions();
	}

	void PhysicsSystem::FirePhysicsEvents()
	{
		CollisionList all_collisions = [&]()
		{
			auto retval = previous_collisions;
			auto clone = collisions;
			retval.merge(clone);
			return retval;
		}();

		using PairList = hash_set<CollisionPair, pair_hasher>;

		auto sz = all_collisions.size();
		PairList col_enter; col_enter.reserve(sz);
		PairList col_stay;  col_stay.reserve(sz);
		PairList col_exit;  col_exit.reserve(sz);
		
		auto& curr_collisions = collisions;

		// find all entering
		for (auto& [pair, collision] : all_collisions)
		{
			// find exiting
			if (curr_collisions.find(pair) == curr_collisions.end())
			{
				if (pair.lhs && pair.rhs)
					col_exit.emplace(pair);
				continue;
			}

			// find entering
			if (previous_collisions.find(pair) == previous_collisions.end())
			{
				col_enter.emplace(pair);
				continue;
			}

			col_stay.emplace(pair);
		}
		previous_collisions = std::move(collisions);
		
		// fire events

		const auto collider_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Collider");;

		const auto FireEvent = [&](const PairList& list, string_view trigger_method, string_view collision_method)
		{
			for (auto& [lhs, rhs] : list)
			{
				if (lhs->is_trigger || rhs->is_trigger)
				{
					// fire lhs events
					for (auto& mb : lhs->GetGameObject()->GetComponents<mono::Behavior>())
					{
						auto obj_type = mb->GetObject().Type();
						IDK_ASSERT(obj_type);
						auto thunk = obj_type->GetThunk(trigger_method, 1);
						if (thunk)
						{
							auto mono_obj = collider_type->Construct();
							mono_obj.Assign("handle", rhs.id);
							thunk->Invoke(mb->GetObject(), mono_obj.Raw());
						}
					}
					// fire rhs events
					for (auto& mb : rhs->GetGameObject()->GetComponents<mono::Behavior>())
					{
						auto obj_type = mb->GetObject().Type();
						IDK_ASSERT(obj_type);
						auto thunk = obj_type->GetThunk(trigger_method, 1);
						if (thunk)
						{
							auto mono_obj = collider_type->Construct();
							mono_obj.Assign("handle", lhs.id);
							thunk->Invoke(mb->GetObject(), mono_obj.Raw());
						}
					}
				}
				else
				{
					// fire collision
					auto col = all_collisions.find(CollisionPair{ lhs, rhs });
					IDK_ASSERT(col != all_collisions.end());


					auto& result = col->second;
					{
						ManagedCollision right_collision
						{
							.collider_id = rhs.id, 
							.normal = result.normal_of_collision, 
							.contact_pt = result.point_of_collision
						};

						for (auto& mb : lhs->GetGameObject()->GetComponents<mono::Behavior>())
						{
							auto obj_type = mb->GetObject().Type();
							IDK_ASSERT(obj_type);
							auto thunk = obj_type->GetThunk(collision_method, 1);
							if (thunk)
								thunk->Invoke(mb->GetObject(), right_collision);
						}
					}

					// fire rhs events
					{
						ManagedCollision left_collision
						{
							.collider_id = lhs.id,
							.normal      = -result.normal_of_collision,
							.contact_pt  = result.point_of_collision
						};

						for (auto& mb : rhs->GetGameObject()->GetComponents<mono::Behavior>())
						{
							auto obj_type = mb->GetObject().Type();
							IDK_ASSERT(obj_type);
							auto thunk = obj_type->GetThunk(collision_method, 1);
							if (thunk)
								thunk->Invoke(mb->GetObject(), left_collision);
						}
					}
				}
			}

		};

		FireEvent(col_enter, "OnTriggerEnter", "OnCollisionEnter");
		FireEvent(col_stay, "OnTriggerStay", "OnCollisionStay");
		FireEvent(col_exit, "OnTriggerExit", "OnCollisionExit");

	}

	void PhysicsSystem::DebugDrawColliders(span<class Collider> colliders)
	{
		// put shape into world space
		constexpr auto calc_shape = [](const auto& shape, Handle<RigidBody> rb, const Collider& col)
		{
			rb;
			return shape * col.GetGameObject()->Transform()->GlobalMatrix();
		};

		constexpr auto debug_draw = [calc_shape](const Collider& collider, const color& c = color{ 1,0,0 }, const seconds& dur = Core::GetDT())
		{
			std::visit([&](const auto& shape)
				{
					Core::GetSystem<DebugRenderer>().Draw(calc_shape(shape, collider.GetGameObject()->GetComponent<RigidBody>(), collider), collider.is_trigger ? color{0,1,1} : c, dur);
				}, collider.shape);
		};

		for (auto& collider : colliders)
			debug_draw(collider);
	}

	void PhysicsSystem::Reset()
	{
		previous_collisions.clear();
		collisions.clear();
	}

	vector<RaycastHit> PhysicsSystem::Raycast(const ray& r, int layer_mask, bool hit_triggers)
	{
		Core::GetSystem<DebugRenderer>().Draw(r, color{1,1,0});

		auto colliders = GameState::GetGameState().GetObjectsOfType<Collider>();
		vector<RaycastHit> retval;

		for (auto& c : colliders)
		{
			{
				auto layer = c.GetGameObject()->GetComponent<Layer>();
				auto mask = layer ? layer->mask() : 1 << 0;
				if (!(mask & layer_mask))
					continue;
			}

			if (c.is_trigger && hit_triggers == false)
				continue;

			auto result = std::visit([&](const auto& shape) -> phys::raycast_result
				{
					using RShape = std::decay_t<decltype(shape)>;

					const auto rShape = calc_shape(shape, c);

					if constexpr (std::is_same_v<RShape, sphere>)
						return phys::collide_ray_sphere(
							r, rShape);
					else
						if constexpr (std::is_same_v<RShape, box>)
							return phys::collide_ray_aabb(
								r, c.bounds());
						else
							return phys::raycast_failure{};
				}, c.shape);

			if (result)
				retval.emplace_back(RaycastHit{ c.GetHandle(), std::move(*result) });
		}

		std::sort(retval.begin(), retval.end(), 
			[](const RaycastHit& lhs, const RaycastHit& rhs) 
			{ 
				return abs(lhs.raycast_succ.distance_to_collision) < abs(rhs.raycast_succ.distance_to_collision);
			}
		);

		return retval;
	}

	bool PhysicsSystem::RayCastAllObj(const ray& r, vector<Handle<GameObject>>& collidedList,vector<phys::raycast_result>& ray_resultList)
	{
		auto colliders = GameState::GetGameState().GetObjectsOfType<Collider>();

		// put shape into world space
		constexpr auto calc_shape = [](const auto& shape, const Collider& col)
		{
			return shape * col.GetGameObject()->Transform()->GlobalMatrix();
		};
		bool foundRes = false;
		
		for (auto& c : colliders)
		{
			const auto result = std::visit([&](const auto& shape) -> phys::raycast_result
			{
				using RShape = std::decay_t<decltype(shape)>;

				const auto rShape = calc_shape(shape, c);

				if constexpr (std::is_same_v<RShape, sphere>)
					return phys::collide_ray_sphere(
						r, rShape);
				else
					if constexpr (std::is_same_v<RShape, box>)
						return phys::collide_ray_aabb(
							r, c.bounds());
				else
					return phys::raycast_failure{};
			}, c.shape);

			if (result)
			{
				//Success
				collidedList.emplace_back(c.GetGameObject());
				ray_resultList.emplace_back(result);
				foundRes = true;
			}
			else
			{
				//Fail
			}
		}

		return foundRes;
	}

	bool PhysicsSystem::RayCastAllObj(const ray& r, vector<Handle<GameObject>>& collidedList)
	{
		auto colliders = GameState::GetGameState().GetObjectsOfType<Collider>();

		// put shape into world space
		constexpr auto calc_shape = [](const auto& shape, const Collider& col)
		{
			return shape * col.GetGameObject()->Transform()->GlobalMatrix();
		};
		bool foundRes = false;

		for (auto& c : colliders)
		{
			const auto result = std::visit([&](const auto& shape) -> phys::raycast_result
				{
					using RShape = std::decay_t<decltype(shape)>;

					const auto rShape = calc_shape(shape, c);

					if constexpr (std::is_same_v<RShape, sphere>)
						return phys::collide_ray_sphere(
							r, rShape);
					else
						if constexpr (std::is_same_v<RShape, box>)
							return phys::collide_ray_aabb(
								r, c.bounds());
						else
							return phys::raycast_failure{};
				}, c.shape);

			if (result)
			{
				//Success
				collidedList.emplace_back(c.GetGameObject());
				foundRes = true;
			}
			else
			{
				//Fail
			}
		}

		return foundRes;
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