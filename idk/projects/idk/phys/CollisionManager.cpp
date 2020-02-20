#include "stdafx.h"

#include <core/GameObject.inl>
#include <common/Transform.h>
#include <common/Layer.h>

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

#include <scene/Scene.h>

namespace idk
{
	constexpr float restitution_slop = 0.01f;
	constexpr float penetration_min_slop = 0.001f;
	constexpr float penetration_max_slop = 0.5f;
	constexpr float damping = 0.99f;
	constexpr float margin = 0.2f;
	constexpr int	collision_threshold = 64;

	void CollisionManager::UpdatePairs(span<class RigidBody> rbs, span<class Collider> colliders, span<class Transform>)
	{

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
					// Check to see if it is static or not
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
				_dynamic_info.emplace_back(ColliderInfo{ .collider = &elem, .layer = elem.GetGameObject()->Layer() });
		}
	}
	void CollisionManager::Reset()
	{
		_dynamic_info.clear();
		_static_broadphase.clear();
		_first_update = true;
	}
	void CollisionManager::BuildStaticTree()
	{
	}
}


