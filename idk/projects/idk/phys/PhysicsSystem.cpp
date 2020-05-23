#include "stdafx.h"
#include "PhysicsSystem.h"
#include <core/GameObject.inl>
#include <common/Transform.h>
#include <common/Layer.h>

#include <gfx/DebugRenderer.h>

#include <scene/SceneManager.h>

#include <phys/RigidBody.h>
#include <phys/Collider.h>
#include <phys/collision_detection/collision_box.h>
#include <phys/collision_detection/collision_sphere.h>
#include <phys/collision_detection/collision_box_sphere.h>
#include <phys/collision_detection/collision_capsule.h>
#include <phys/collision_detection/collision_capsule_box.h>
#include <phys/collision_detection/collision_capsule_sphere.h>
#include <phys/collision_detection/ManagedCollision.h>
#include <phys/collision_contact.h>

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
	void PhysicsSystem::SimulateOneObject(Handle<class RigidBody> rb)
	{
		_col_manager.SimulateOneObject(rb);
		FirePhysicsEvents();
	}
	// #pragma optimize("", off)
	void PhysicsSystem::PhysicsTick(span<class RigidBody> rbs, span<class Collider> colliders, span<class Transform>)
	{
		const auto dt = Core::GetDT().count();
		if (_rebuild_tree)
		{
			_col_manager.BuildStaticTree(colliders);
			_rebuild_tree = false;
		}

		// New frame will insert new static objects into the tree and also initialize the dynamic info
		_col_manager.InitializeNewFrame(rbs, colliders);
		_col_manager.ApplyGravityAndForces(rbs);
		
		// Physics is fine with just 1 pass.
		// for (int i = 0; i < 3; ++i)
		{
			// Cache global tfm and global rotations as well as update broadphase shapes.
			// Will also compute intertia tensors here.
			_col_manager.UpdateDynamics();
			_col_manager.TestCollisions();
			if (debug_draw_colliders)
				_col_manager.DebugDrawContactPoints(dt);
			_col_manager.PreSolve();
			_col_manager.Solve();
		}
		_col_manager.Finalize(rbs);

		if (!debug_draw_colliders)
			return;
		_col_manager.DebugDrawColliders(colliders, dt);
	}

	void PhysicsSystem::FirePhysicsEvents()
	{
		auto& curr_collisions = _col_manager._collisions;
		CollisionList all_collisions = [&]()
		{
			auto retval = _prev_collisions;
			auto clone = curr_collisions;
			retval.merge(clone);
			return retval;
		}();

		using PairList = hash_set<CollisionPair, pair_hasher>;

		auto sz = all_collisions.size();
		PairList col_enter; col_enter.reserve(sz);
		PairList col_stay;  col_stay.reserve(sz);
		PairList col_exit;  col_exit.reserve(sz);
		
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
			if (_prev_collisions.find(pair) == _prev_collisions.end())
			{
				col_enter.emplace(pair);
				continue;
			}

			col_stay.emplace(pair);
		}
		_prev_collisions = _col_manager._collisions;
		
		// fire events

		const auto collider_type = Core::GetSystem<mono::ScriptSystem>().Environment().Type("Collider");

		const auto FireEvent = [&](const PairList& list, string_view trigger_method, string_view collision_method)
		{
			for (auto& [lhs, rhs] : list)
			{
				if (!(lhs && rhs))
					continue;

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
							.normal = result.normal, 
							.contact_pt = result.contact_centroid
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
							.normal      = -result.normal,
							.contact_pt  = result.contact_centroid
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
		const float a = collider.enabled ? 1.0f : 0.25f;
		const auto col = (collider.is_trigger ? color{ 0,1,1 } : color{ 1,0,0 }) * a;
        std::visit([&](const auto& shape)
        {
				Core::GetSystem<DebugRenderer>().Draw(calc_shape(shape, collider), col, Core::GetDT() + seconds{0.02f});
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
		_prev_collisions.clear();
		_col_manager.Reset();
		_rebuild_tree = true;
	}

	vector<RaycastHit> PhysicsSystem::Raycast(const ray& r, LayerMask layer_mask, bool hit_triggers)
	{
		Core::GetSystem<DebugRenderer>().Draw(r, color{ 1,1,0 });

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

	void PhysicsSystem::Init()
	{
		_col_manager.Init();
		Core::GetSystem<SceneManager>().OnSceneChange += [&](RscHandle<Scene>) { Reset(); };
	}

	void PhysicsSystem::Shutdown()
	{
	}

}