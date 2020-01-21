#include "stdafx.h"
#include "PhysicsSystem.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <common/Layer.h>
#include <gfx/DebugRenderer.h>
#include <phys/RigidBody.h>
#include <phys/Collider.h>
#include <phys/collision_detection/collision_box.h>
#include <phys/collision_detection/collision_sphere.h>
#include <phys/collision_detection/collision_box_sphere.h>
#include <phys/collision_detection/collision_capsule.h>
#include <phys/collision_detection/collision_capsule_box.h>
#include <phys/collision_detection/collision_capsule_sphere.h>
#include <phys/collision_detection/ManagedCollision.h>

#include <script/ManagedObj.inl>
#include <script/MonoBehavior.h>
#include <script/MonoFunctionInvoker.h>
#include <script/ScriptSystem.h>

#include <math/matrix_decomposition.inl>

#include <parallel/ThreadPool.h>
#include <ds/span.inl>
#include <ds/result.inl>

namespace idk
{
#define OCTREE_ENABLE 1
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

#if OCTREE_ENABLE
		if (octree_cleared)
		{
			BuildOctree(colliders);
			octree_cleared = false;
		}
		vector<Handle<Collider>> dynamic_info;

		for (auto& elem : colliders)
		{
			elem._active_cache = elem.is_enabled_and_active();
			if (!elem._active_cache || elem.GetHandle().scene == Scene::prefab)
				continue;

			elem.find_rigidbody();

			auto collider_handle = elem.GetHandle();
			auto res = elem.get_octree_node()->object_list.find(collider_handle);

			if (res != elem.get_octree_node()->object_list.end())
			{
				res->second.layer = elem.GetGameObject()->Layer();

				// Update all static objects in the octree
				if (elem._static_cache)
				{
					auto info = std::visit([&elem](const auto& shape) -> collider_info {
						auto pred_shape = shape * elem.GetGameObject()->Transform()->GlobalMatrix();
						return collider_info{
							.bound = pred_shape.bounds(),
							.predicted_shape = pred_shape
						};
						}, elem.shape);

					info.collider = collider_handle;
					res->second.bound = info.bound;
					res->second.predicted_shape = info.predicted_shape;

					// NOTE: res potentially null after this due to the object being moved
					UpdateOctree(info, elem._octree_node);
				}
				else
				{
					dynamic_info.emplace_back(collider_handle);
				}
				
			}
		}
#else
		vector<ColliderInfo> static_info;
		vector<ColliderInfo> dynamic_info;

		static_info.reserve(colliders.size() - rbs.size());

		for (auto& elem : colliders)
		{
			elem._active_cache = elem.is_enabled_and_active();
			if (!elem._active_cache || elem.GetHandle().scene == Scene::prefab)
				continue;

			elem.find_rigidbody();
			if (elem._static_cache)
			{

				static_info.emplace_back(
					std::visit([&elem](const auto& shape) -> ColliderInfo {
						auto pred_shape = shape * elem.GetGameObject()->Transform()->GlobalMatrix();
						return ColliderInfo{ elem, pred_shape.bounds(), pred_shape, elem.GetGameObject()->Layer() };
						}, elem.shape)
				);

			}
			else
				dynamic_info.emplace_back(ColliderInfo{ .collider = elem, .layer = elem.GetGameObject()->Layer() });
		}
#endif

		constexpr auto debug_draw = [](const CollidableShapes& pred_shape, const color& c = color{ 1,0,0 }, const seconds& dur = Core::GetDT())
		{
			std::visit([&](const auto& shape)
			{
				Core::GetSystem<DebugRenderer>().Draw(shape, c, dur);
			}, pred_shape);
		};


		// phases
		const auto ApplyGravity = [&]()
		{
			for (auto& rb : rbs)
			{
				if (rb.sleeping())
					continue;
				if (rb.use_gravity && !rb.is_kinematic)
                    rb.AddForce(vec3{ 0, -9.81, 0 });
			}
		};

		const auto PredictTransform = [&]()
		{
			const auto dt = Core::GetDT().count();
			const auto half_dt = dt / 2;

			for (auto& rigidbody : rbs)
			{
				const auto tfm = rigidbody.GetGameObject()->Transform();
                rigidbody._pred_tfm = tfm->GlobalMatrix();

                if (rigidbody.sleeping())
                {
                    rigidbody._prev_pos = rigidbody._pred_tfm[3].xyz - rigidbody.initial_velocity * dt;
                    rigidbody.initial_velocity = vec3{};
                }
                else if (!rigidbody.is_kinematic)
				{
					const vec3 curr_pos = rigidbody._pred_tfm[3].xyz;

					// verlet integrate towards new position
					//auto new_pos = curr_pos + (curr_pos - rigidbody._prev_pos)*(damping) + rigidbody._accum_accel * dt * dt;
					auto new_pos = 2.f * curr_pos - rigidbody._prev_pos + rigidbody._accum_accel * dt * dt;
					rigidbody._accum_accel = vec3{};
					rigidbody._prev_pos = curr_pos;
                    rigidbody._pred_tfm[3].xyz = new_pos;
				}
			}
		};

        constexpr auto CollideShapes = [](const auto& lshape, const auto& rshape) -> phys::col_result
	    {
		    using LShape = std::decay_t<decltype(lshape)>;
		    using RShape = std::decay_t<decltype(rshape)>;

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
		    if constexpr (std::is_same_v<LShape, capsule>&& std::is_same_v<RShape, box>)
			    return phys::collide_capsule_box_discrete(
				    lshape, rshape);
		    else
		    if constexpr (std::is_same_v<LShape, box>&& std::is_same_v<RShape, capsule>)
			    return phys::collide_capsule_box_discrete(
				    rshape, lshape);
		    else
		    if constexpr (std::is_same_v<LShape, capsule>&& std::is_same_v<RShape, capsule>)
			    return phys::collide_capsule_capsule_discrete(
				    lshape, rshape);
		    else
			    return phys::col_failure{};
	    };

		const auto PredictAndUpdateDynamic = [&]()
		{
			// Update all dynamic colliders in the octree
			for (auto& info : dynamic_info)
			{
				collider_info new_info = info->get_octree_node()->object_list.find(info)->second;

				new_info.predicted_shape = std::visit([&pred_tfm = new_info.collider->_rigidbody->_pred_tfm](const auto& shape)->CollidableShapes { return shape * pred_tfm; }, new_info.collider->shape);
				new_info.bound = std::visit([&pred_tfm = new_info.collider->_rigidbody->_pred_tfm](const auto& shape) { return (shape * pred_tfm).bounds(); }, new_info.collider->shape);
				const auto& vel = new_info.collider->_rigidbody->velocity();
				new_info.bound.grow(vel);
				new_info.bound.grow(-vel);

				UpdateOctree(new_info, info->get_octree_node());
			}
		};

		const auto CollideObjects = [&]()
		{
            
			const auto dt = Core::GetDT().count();
#if OCTREE_ENABLE
			// Update all dynamic stuff in the octree
			PredictAndUpdateDynamic();

			vector<CollisionInfo> collision_frame;
			collision_frame.reserve(dynamic_info.size()); //guess

			
			
			auto info = PairColliders();
			// For each node in octree
				// For each item in node
					// Pair with every other node in octree

#else
			vector<CollisionInfo> collision_frame;
			collision_frame.reserve(dynamic_info.size()); //guess

            // setup predict for dynamic objects
            for (auto& info : dynamic_info)
            {
                info.predicted_shape = std::visit([&pred_tfm = info.collider._rigidbody->_pred_tfm](const auto& shape) -> CollidableShapes { return shape * pred_tfm; }, info.collider.shape);
                info.broad_phase = std::visit([&pred_tfm = info.collider._rigidbody->_pred_tfm](const auto& shape) { return (shape * pred_tfm).bounds(); }, info.collider.shape);
                const auto& vel = info.collider._rigidbody->velocity();
                info.broad_phase.grow(vel);
                info.broad_phase.grow(-vel);
            }

            // O(N^2) collision check
            // all objects confirmed to be active (but may be sleeping)

			

			vector<ColliderInfoPair> info;
			info.reserve(dynamic_info.size() * 4);

            // dynamic vs dynamic
            for (const auto& i : dynamic_info)
            {
                for (const auto& j : dynamic_info)
                {
                    const auto& lrigidbody = *i.collider._rigidbody;
                    const auto& rrigidbody = *j.collider._rigidbody;

                    if (lrigidbody.GetHandle() == rrigidbody.GetHandle())
                        continue;
                    if (lrigidbody.sleeping() && rrigidbody.sleeping())
                        continue;
                    if (!AreLayersCollidable(i.layer, j.layer))
                        continue;
                    if (!i.broad_phase.overlaps(j.broad_phase))
                        continue;

					info.emplace_back(ColliderInfoPair{ &i, &j });
					/*
                    const auto collision = std::visit(CollideShapes, i.predicted_shape, j.predicted_shape);
                    if (collision)
                    {
                        collision_frame.emplace_back(CollisionInfo{ i, j, collision.value() });
                        collisions.emplace(CollisionPair{ i.collider.GetHandle(), j.collider.GetHandle() }, collision.value());
                    }
					*/
                }
            }
            // dynamic vs static
            for (const auto& i : dynamic_info)
            {
                for (const auto& j : static_info)
                {
                    const auto& lrigidbody = *i.collider._rigidbody;

                    if (lrigidbody.sleeping())
                        continue;
                    if (!AreLayersCollidable(i.layer, j.layer))
                        continue;
                    if (!i.broad_phase.overlaps(j.broad_phase))
                        continue;
					info.emplace_back(ColliderInfoPair{ &i,&j });
					/*
                    const auto collision = std::visit(CollideShapes, i.predicted_shape, j.predicted_shape);
                    if (collision)
                    {
                        collision_frame.emplace_back(CollisionInfo{ i, j, collision.value() });
                        collisions.emplace(CollisionPair{ i.collider.GetHandle(), j.collider.GetHandle() }, collision.value());
                    }
					*/
                }
            }
#endif
			using CollisionJobResult = std::tuple<vector<CollisionInfo>, CollisionList>;
			vector<mt::Future<CollisionJobResult>> batches;

			if (info.size())
			{
				const auto sz = info.size();
				const auto batch_sz = GetConfig().batch_size;
				batches.reserve(batch_sz / sz + batch_sz % sz ? 1 : 0);

				for (size_t i = 0; i < sz; i += batch_sz)
				{
					batches.push_back(Core::GetThreadPool().Post(
						[batch_sz, CollideShapes](ColliderInfoPair* begin_itr, ColliderInfoPair* end_itr) -> CollisionJobResult
						{
							vector<CollisionInfo> collision_frame;
							CollisionList batch_collisions;
							collision_frame.reserve(batch_sz);
							batch_collisions.reserve(batch_sz);
							while (begin_itr != end_itr)
							{
								auto& [i, j] = *begin_itr;
								const auto collision = std::visit(CollideShapes, i->predicted_shape, j->predicted_shape);
								if (collision)
								{
									collision_frame.emplace_back(CollisionInfo{ i, j, collision.value() });
									batch_collisions.emplace(CollisionPair{ i->collider, j->collider }, collision.value());
								}
								++begin_itr;
							}
							return std::make_tuple(collision_frame, batch_collisions);
						},
						info.data() + i,
						info.data() + std::min(i + batch_sz, sz)
						));
				}

				for (auto& elem : batches)
				{
					auto [batch_frame, collision_list] = elem.get();
					collision_frame.insert(collision_frame.end(), batch_frame.begin(), batch_frame.end());
					collisions.merge(collision_list);
				}
			}
			for (const auto& [i, j, result] : collision_frame)
			{
				const auto& lcollider = *(i->collider);
				const auto& rcollider = *(j->collider);

				auto lrigidbody = lcollider._rigidbody;
				auto rrigidbody = rcollider._rigidbody;

				// triggers do not require resolution
				if (lcollider.is_trigger || rcollider.is_trigger)
					continue;

				struct RigidBodyInfo
				{
					vec3 velocity = {};
					real inv_mass = 0.f;
					RigidBody* ref = nullptr;
				};

				const auto [lvel, linv_mass, lrb_ptr] =
                    RigidBodyInfo{ lrigidbody->_pred_tfm[3].xyz - lrigidbody->_prev_pos, lrigidbody->inv_mass, &*lrigidbody };
                const auto [rvel, rinv_mass, rrb_ptr] = rrigidbody ?
                    RigidBodyInfo{ rrigidbody->_pred_tfm[3].xyz - rrigidbody->_prev_pos, rrigidbody->inv_mass, &*rrigidbody } : RigidBodyInfo{};

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
                        auto& predicted_pos = lrb_ptr->_pred_tfm[3].xyz;
						predicted_pos = predicted_pos + correction_vector;
						const auto new_vel = lvel + (collision_impulse + frictional_impulse) * lrb_ptr->inv_mass;
                        lrb_ptr->_prev_pos = predicted_pos - new_vel;
					}

					if (rrb_ptr && !rrb_ptr->is_kinematic)
					{
                        auto& predicted_pos = rrb_ptr->_pred_tfm[3].xyz;
                        predicted_pos = predicted_pos - correction_vector;
						const auto new_vel = rvel - (collision_impulse + frictional_impulse) * rrb_ptr->inv_mass;
                        rrb_ptr->_prev_pos = predicted_pos - new_vel;
					}
				}
			}
		};

		const auto FinalizePositions = [&]()
		{
            for (const auto& elem : dynamic_info)
            {
                auto& rigidbody = *elem->_rigidbody;
                if (!rigidbody.is_kinematic)
                    rigidbody.GetGameObject()->Transform()->GlobalMatrix(rigidbody._pred_tfm);
                else
                    rigidbody._prev_pos = rigidbody._pred_tfm[3].xyz;
                rigidbody.sleep_next_frame = false;
            }
		};

		collisions.clear();

		ApplyGravity();
		PredictTransform();
		for (int i = 0; i < 4;++i)
			CollideObjects();
		FinalizePositions();

        if (!debug_draw_colliders)
            return;

        // for (const auto& elem : static_info)
        //     debug_draw(elem.predicted_shape, elem.collider.is_trigger ? color{ 0, 1, 1 } : color{ 1, 0, 0 });
        // for (const auto& elem : dynamic_info)
        //     debug_draw(elem.predicted_shape, elem.collider.is_trigger ? color{ 0, 1, 1 } : color{ 1, 0, 0 });
		DrawOctreeDebug(_collider_octree._root);
        for (auto& elem : colliders)
        {
            if (!elem._active_cache)
            {
                std::visit([&](const auto& shape)
                {
                    debug_draw(calc_shape(shape, elem), color{ 0.5 });
                }, elem.shape);
            }
        }
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

    void PhysicsSystem::DrawCollider(const Collider& collider) const
    {
        std::visit([&](const auto& shape)
        {
            Core::GetSystem<DebugRenderer>().Draw(calc_shape(shape, collider),
                collider.is_trigger ? color{ 0,1,1 } : color{ 1,0,0 }, Core::GetDT());
        }, collider.shape);
    }

	void PhysicsSystem::DebugDrawColliders(span<class Collider> colliders)
	{
        if (!debug_draw_colliders)
            return;
		for (auto& collider : colliders)
            DrawCollider(collider);
	}

	void PhysicsSystem::Reset()
	{
		octree_cleared = true;
		_collider_octree.clear();
		previous_collisions.clear();
		collisions.clear();
	}

	vector<RaycastHit> PhysicsSystem::Raycast(const ray& r, LayerMask layer_mask, bool hit_triggers)
	{
		Core::GetSystem<DebugRenderer>().Draw(r, color{1,1,0});

		auto colliders = GameState::GetGameState().GetObjectsOfType<Collider>();
		vector<RaycastHit> retval;

		for (auto& c : colliders)
		{
			{
				auto layer = c.GetGameObject()->GetComponent<Layer>();
				auto mask = layer ? layer->mask() : LayerMask{ 1 << 0 };
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
							return phys::collide_ray_box(
								r, rShape);
					else
						if constexpr (std::is_same_v<RShape, capsule>)
							return phys::collide_ray_capsule(
								r, rShape);
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
		/*constexpr auto calc_shape = [](const auto& shape, const Collider& col)
		{
			return shape * col.GetGameObject()->Transform()->GlobalMatrix();
		};*/
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
					if constexpr (std::is_same_v<RShape, capsule>)
						return phys::collide_ray_capsule(
							r, rShape);
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
		/*constexpr auto calc_shape = [](const auto& shape, const Collider& col)
		{
			return shape * col.GetGameObject()->Transform()->GlobalMatrix();
		};*/
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
						if constexpr (std::is_same_v<RShape, capsule>)
							return phys::collide_ray_capsule(
								r, rShape);
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

    bool PhysicsSystem::AreLayersCollidable(LayerManager::layer_t a, LayerManager::layer_t b) const
    {
        return GetConfig().matrix[a] & LayerMask(1 << b);
    }


	void PhysicsSystem::DrawOctreeDebug(shared_ptr<octree_node> node)
	{
		if (node)
		{
			box b{
				.center = node->bound.center(),
				.extents = node->bound.extents()
			};
			Core::GetSystem<DebugRenderer>().Draw(b, color{ 0,1,0,1 });

			for (auto& child : node->children)
			{
				DrawOctreeDebug(child);
			}
		}
	}

	void PhysicsSystem::BuildOctree(span<Collider> colliders)
	{
		// Get the bounding box that bounds all the colliders
		if (!colliders.empty())
		{
			auto bounds = colliders[0].bounds();
			for (auto& col : colliders)
			{
				bounds.surround(col.bounds());
			}
			static const vec3 grow_dir{ 1.0f / sqrtf(3.0f) };
			const auto center = bounds.center();
			const auto extents = bounds.extents();
			const auto width = max(max(extents.x, extents.y), extents.z);

			_collider_octree.clear();
			_collider_octree.rebuild(bounds.center(), width, 0);

			for (auto& col : colliders)
			{
				collider_info info
				{
					.collider = col.GetHandle(),
					.bound = col.bounds()
				};

				_collider_octree.insert(info);
			}
		}
	}

	void PhysicsSystem::UpdateOctree(collider_info& col, shared_ptr<octree_node> node)
	{
		if (!node->bound.contains(col.bound))
		{
			_collider_octree.erase_from(col.collider, node);
			_collider_octree.insert(col);
		}
		// try to descend the tree (not straddling anymore
		else
		{
			_collider_octree.descend(node, col);
		}
	}

	vector<PhysicsSystem::ColliderInfoPair> PhysicsSystem::PairColliders()
	{
		vector<ColliderInfoPair> info;
		info.reserve(_collider_octree.object_count * 2);
		PairColliders(_collider_octree._root, info);
		return info;
	}

	void PhysicsSystem::PairColliders(shared_ptr<octree_node> node, vector<ColliderInfoPair>& pairs)
	{
		if (node)
		{
			auto all_info = _collider_octree.get_info(node);
			auto node_obj_count = node->object_list.size();

			auto end_it = all_info.end();
			for (size_t i = 0; i < node_obj_count; ++i)
			{
				auto it = all_info.begin() + i;
				auto itr = *it;
				auto lhs = itr->collider;

				auto jt = it;
				for (++jt; jt != end_it; ++jt)
				{
					auto jtr = *jt;
					auto rhs = jtr->collider;

					// No need to check static vs static
					if (lhs->_static_cache && rhs->_static_cache)
						continue;

					bool lhs_check = true;
					if (!lhs->_static_cache)
						lhs_check = !lhs->_rigidbody->sleeping();

					bool rhs_check = true;
					if (!rhs->_static_cache)
						rhs_check = !rhs->_rigidbody->sleeping();

					if (!lhs_check && !rhs_check)
						continue;
					
					if (!AreLayersCollidable(itr->layer, jtr->layer))
						continue;

					pairs.emplace_back(ColliderInfoPair{ itr, jtr });
				}
			}

			for (auto& child : node->children)
			{
				PairColliders(child, pairs);
			}
		}
	}

	void PhysicsSystem::Init()
	{
		GameState::GetGameState().OnObjectCreate<Collider>() += [&](Handle<Collider> collider)
		{
			if (!collider || octree_cleared)
				return;
		
			collider_info info;
			info.collider = collider;
			info.bound = collider->bounds();
			_collider_octree.insert(info);
		};
		
		GameState::GetGameState().OnObjectDestroy<Collider>() += [&](Handle<Collider> collider)
		{
			if (!collider || octree_cleared)
				return;
		
			_collider_octree.erase_from(collider, collider->get_octree_node());
		};
	}

	void PhysicsSystem::Shutdown()
	{
	}

	void PhysicsSystem::BuildOctree()
	{
		auto colliders = GameState::GetGameState().GetObjectsOfType<Collider>();
		BuildOctree(colliders);
	}

	void PhysicsSystem::ClearOctree()
	{
		_collider_octree.clear();
	}

	size_t PhysicsSystem::pair_hasher::operator()(const CollisionPair& collision_pair) const
	{
		auto hash = std::hash<size_t>{}(collision_pair.lhs.id);
		hash_combine(hash, collision_pair.rhs.id);
		return hash;
	}
}