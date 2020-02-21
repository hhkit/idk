#include "stdafx.h"

#include <core/GameObject.inl>
#include <common/Transform.h>
#include <common/Layer.h>

#include <phys/PhysicsSystem.h>
#include <phys/CollisionManager.h>
#include <phys/ContactSolver.h>
#include <phys/Collider.h>
#include <phys/RigidBody.h>
#include <phys/Collision_utils.h>
#include <phys/collision_result.h>
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

#include <scene/Scene.h>

namespace idk
{
	constexpr float restitution_slop = 0.01f;
	constexpr float penetration_min_slop = 0.001f;
	constexpr float penetration_max_slop = 0.5f;
	constexpr float damping = 0.99f;
	constexpr float margin = 0.2f;
	constexpr int	collision_threshold = 64;

	constexpr auto calc_shape = [](const auto& shape, const Collider& col)
	{
		return shape * col.GetGameObject()->Transform()->GlobalMatrix();
	};

	void CollisionManager::UpdatePairs(span<class RigidBody> rbs, span<class Collider> colliders)
	{
		const auto dt = Core::GetDT().count();
		_dynamic_info.clear();
		for (auto& elem : colliders)
		{
			// bool to_insert = false;
			bool to_insert = false;
			bool to_remove = false;

			// Active/Enabled
			const bool is_active = elem.is_enabled_and_active();
			const bool was_active = elem._active_cache;
			elem._active_cache = is_active;

			// Static/non-static
			const bool was_static = elem._static_cache;
			elem.find_rigidbody();
			const bool is_static = elem._static_cache;

			const bool is_prefab = elem.GetHandle().scene == Scene::prefab;
			// Update tree based on colliders that are active
			if (!is_prefab)
			{
				// deactivated this frame
				if (was_active && !is_active)
				{
					// make sure its static
					if (is_static || was_static)
					{
						to_remove = true;
					}
				}
				// Activated this frame
				else if (is_active && !was_active)
				{
					to_insert = true;
				}
				// Was neither activated nor deactivated
				else if (is_active)
				{
					// Check if the had a rigidbody added to it
					if (was_static && !is_static)
					{
						to_remove = true;

					}
					// Rigidbody removed
					else if (is_static && !was_static)
					{
						to_insert = true;
					}
				}
			}

			if (to_remove)
			{
				if (_static_broadphase.remove(elem.node_id))
				{
					// LOG_TO(LogPool::PHYS, "REMOVED from tick: %u, %u, %u", elem.GetHandle().index, elem.GetHandle().gen, elem.GetHandle().scene);
					elem.node_id = -1;
				}
			}

			if (!is_active || is_prefab)
				continue;

			if (is_static)
			{
				const auto collider_info = std::visit([&elem](const auto& shape) -> ColliderInfo {
					auto pred_shape = shape * elem.GetGameObject()->Transform()->GlobalMatrix();
					return ColliderInfo{ .collider = &elem, .broad_phase = pred_shape.bounds(), .predicted_shape = pred_shape, .layer = elem.GetGameObject()->Layer() };
					}, elem.shape);
				// static_info.emplace_back(collider_info);
				if (to_insert)
				{
					// inserted_within_tick.emplace(elem.GetHandle());
					// LOG_TO(LogPool::PHYS, "INSERTED from TICK: %u, %u, %u", elem.GetHandle().index, elem.GetHandle().gen, elem.GetHandle().scene);
					_static_broadphase.insert(elem, collider_info, margin);
				}
				else
					_static_broadphase.update(elem, collider_info, margin);
			}
			else
			{
				ColliderInfo d_info{ .collider = &elem, .layer = elem.GetGameObject()->Layer() };
				d_info.predicted_shape = std::visit([&pred_tfm = d_info.collider->_rigidbody->_pred_tfm](const auto& shape)->CollidableShapes { return shape * pred_tfm; }, d_info.collider->shape);
				d_info.broad_phase = std::visit([&pred_tfm = d_info.collider->_rigidbody->_pred_tfm](const auto& shape) { return (shape * pred_tfm).bounds(); }, d_info.collider->shape);
				auto vel = d_info.collider->_rigidbody->velocity() / dt;
				d_info.broad_phase.grow(vel);
				d_info.broad_phase.grow(-vel);
				_dynamic_info.emplace_back(d_info);
			}
		}
	}

	void CollisionManager::TestCollisions()
	{
		auto& phys = Core::GetSystem<PhysicsSystem>();
		_info.clear();
		_info.reserve(_dynamic_info.size() * 4);

		// Dynamic vs Dynamic Broadphase
		auto dynamic_end = _dynamic_info.end();
		for (auto i = _dynamic_info.begin(); i != dynamic_end; ++i)
		{
			const auto& lrigidbody = *i->collider->_rigidbody;
			const bool lrb_sleeping = lrigidbody.sleeping();
			for (auto j = i + 1; j != dynamic_end; ++j)
			{
				const auto& rrigidbody = *j->collider->_rigidbody;

				if (lrb_sleeping && rrigidbody.sleeping())
					continue;
				if (!phys.AreLayersCollidable(i->layer, j->layer))
					continue;
				if (!i->broad_phase.overlaps(j->broad_phase))
					continue;

				_info.emplace_back(ColliderInfoPair{ &*i, &*j });
			}
		}

		// Static vs Dynamic Broadphase
		for (const auto& i : _dynamic_info)
		{
			const auto& lrigidbody = *i.collider->_rigidbody;

			if (lrigidbody.sleeping())
				continue;
			_static_broadphase.query_collisions(i, _info);
		}

		// Narrow phase.
		_collisions.clear();
		_collisions.reserve(_info.size());
		for (auto& pair : _info)
		{
			auto& i = pair.lhs;
			auto& j = pair.rhs;
			const auto collision = CollideShapes(i->predicted_shape, j->predicted_shape);
			if (collision)
			{
				_collisions.emplace(CollisionPair{ i->collider->GetHandle(), j->collider->GetHandle() }, collision.value());
			}
		}
	}

	void CollisionManager::Init()
	{
		_static_broadphase.preallocate_nodes(2500); // Avg ~1030 static objects -> means (2 * 1030 - 1) total nodes in b-tree

		GameState::GetGameState().OnObjectCreate<Collider>() += [&](Handle<Collider> collider)
		{
			if (!collider)
				return;

			auto& col = *collider;
			col._static_cache = col.is_enabled_and_active();
			if (!col._static_cache || col.GetHandle().scene == Scene::prefab)
				return;

			col.find_rigidbody();
			if (col._static_cache)
			{
				_static_broadphase.insert(col, col.bounds(), 0.2f);
				// LOG_TO(LogPool::PHYS, "INSERTED from object create: %u, %u, %u", collider.index, collider.gen, collider.scene);
			}
		};

		GameState::GetGameState().OnObjectDestroy<Collider>() += [&](Handle<Collider> collider)
		{
			if (!collider)
				return;

			auto& col = *collider;
			if (!col._static_cache)
				return;

			if (_static_broadphase.remove(col.node_id))
			{
				// LOG_TO(LogPool::PHYS, "REMOVED from object create: %u, %u, %u", collider.index, collider.gen, collider.scene);
				col.node_id = -1;
				col._active_cache = false;
			}
		};
	}

	void CollisionManager::Reset()
	{
		_dynamic_info.clear();
		_collisions.clear();
		_info.clear();
		_static_broadphase.clear();
	}

	void CollisionManager::DebugDrawContactPoints(float dt)
	{
		auto& dbg = Core::GetSystem<DebugRenderer>();
		for (auto col : _collisions)
		{
			for (int i = 0; i < col.second.manifold.contactCount; ++i)
			{
				const auto& c = col.second.manifold.contacts[i];
				dbg.Draw(c.position, color{ 0,1,0,1 }, seconds{ 0.5f });
				dbg.Draw(ray{ c.position, col.second.manifold.normal }, color{ 0,1,0,1 }, seconds{ dt });
				dbg.Draw(ray{ c.position, col.second.manifold.tangentVectors[0] }, color{ 0,0,1,1 }, seconds{ dt });
				dbg.Draw(ray{ c.position, col.second.manifold.tangentVectors[0] }, color{ 0,0,1,1 }, seconds{ dt });
			}
			LOG_TO(LogPool::PHYS, "Contact Count: %d", col.second.manifold.contactCount);
		}
	}

	void CollisionManager::DebugDrawColliders(span<class Collider> colliders, float dt)
	{
		auto& dbg = Core::GetSystem<DebugRenderer>();
		constexpr auto debug_draw = [](const CollidableShapes& pred_shape, const color& c = color{ 1,0,0 }, const seconds& dur = Core::GetDT())
		{
			std::visit([&](const auto& shape)
				{
					dbg.Draw(shape, c, dur);
				}, pred_shape);
		};

		_static_broadphase.debug_draw();
		for (const auto& elem : _dynamic_info)
			debug_draw(elem.predicted_shape, elem.collider->is_trigger ? color{ 0, 1, 1 } : color{ 1, 0, 0 }, seconds{ 0.5f });
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

	void CollisionManager::BuildStaticTree()
	{
		auto colliders = GameState::GetGameState().GetObjectsOfType<Collider>();
		BuildStaticTree(colliders);
	}

	void CollisionManager::BuildStaticTree(span<Collider> colliders)
	{
		_static_broadphase.clear();
		for (auto& col : colliders)
		{
			col._active_cache = col.is_enabled_and_active();
			if (!col._active_cache || col.GetHandle().scene == Scene::prefab)
				return;

			col.find_rigidbody();
			if (col._static_cache)
			{
				_static_broadphase.insert(col, col.bounds(), 0.2f);
			}
		}
	}
	phys::col_result CollisionManager::CollideShapes(const CollidableShapes& lhs, const CollidableShapes& rhs)
	{
		constexpr auto Collide = [](const auto& lshape, const auto& rshape) -> phys::col_result
	    {
		    using LShape = std::decay_t<decltype(lshape)>;
		    using RShape = std::decay_t<decltype(rshape)>;

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

		return std::visit(Collide, lhs, rhs);
	}
}


