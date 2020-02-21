#pragma once
#include <idk.h>
#include <phys/Collider.h>
#include <phys/collidable_shapes.h>
#include <common/LayerManager.h>

namespace idk
{
	struct ColliderInfo
	{
		Collider* collider;
		aabb broad_phase;
		CollidableShapes predicted_shape;
		LayerManager::layer_t layer;
	};

	struct ColliderInfoPair
	{
		const ColliderInfo* lhs;
		const ColliderInfo* rhs;
	};

	struct CollisionPair { Handle<Collider> lhs, rhs; auto operator<=>(const CollisionPair&) const = default; };

	struct pair_hasher { size_t operator()(const CollisionPair&) const; };
	using CollisionList = hash_table<CollisionPair, phys::col_success, pair_hasher>;
}