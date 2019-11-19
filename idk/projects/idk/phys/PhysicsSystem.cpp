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
#include <phys/collision_detection/collision_capsule_box.h>
#include <phys/collision_detection/collision_capsule_sphere.h>
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
        struct ColliderInfo
        {
            Collider& collider;
            aabb broad_phase;
            CollidableShapes predicted_shape;
            LayerManager::layer_t layer;
        };

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

		const auto CollideObjects = [&]()
		{
            struct CollisionInfo
            {
                const ColliderInfo& a;
                const ColliderInfo& b;
                phys::col_success res;
            };
			vector<CollisionInfo> collision_frame;
            collision_frame.reserve(dynamic_info.size()); //guess

			const auto dt = Core::GetDT().count();

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

                    const auto collision = std::visit(CollideShapes, i.predicted_shape, j.predicted_shape);
                    if (collision)
                    {
                        collision_frame.emplace_back(CollisionInfo{ i, j, collision.value() });
                        collisions.emplace(CollisionPair{ i.collider.GetHandle(), j.collider.GetHandle() }, collision.value());
                    }
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

                    const auto collision = std::visit(CollideShapes, i.predicted_shape, j.predicted_shape);
                    if (collision)
                    {
                        collision_frame.emplace_back(CollisionInfo{ i, j, collision.value() });
                        collisions.emplace(CollisionPair{ i.collider.GetHandle(), j.collider.GetHandle() }, collision.value());
                    }
                }
            }

			for (const auto& [i, j, result] : collision_frame)
			{
				const auto& lcollider = i.collider;
				const auto& rcollider = j.collider;

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
                auto& rigidbody = *elem.collider._rigidbody;
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

        for (const auto& elem : static_info)
            debug_draw(elem.predicted_shape, elem.collider.is_trigger ? color{ 0, 1, 1 } : color{ 1, 0, 0 });
        for (const auto& elem : dynamic_info)
            debug_draw(elem.predicted_shape, elem.collider.is_trigger ? color{ 0, 1, 1 } : color{ 1, 0, 0 });
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
						if constexpr (std::is_same_v<RShape, capsule>)
							return phys::collide_ray_capsule(
								r, shape);
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
							r, shape);
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
								r, shape);
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
        return GetConfig().matrix[a] & (1 << b);
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