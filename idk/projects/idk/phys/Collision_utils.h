#pragma once
#include <idk.h>
#include <phys/Collider.h>
#include <phys/collidable_shapes.h>
#include <common/LayerManager.h>
#include <phys/collision_result.h>
#include <compare>

namespace idk
{
	struct ColliderInfo
	{
		Collider* collider{ nullptr };
		RigidBody* rb{ nullptr };
		aabb broad_phase{};
		CollidableShapes predicted_shape{};
		LayerManager::layer_t layer{};
	};

	struct ColliderInfoPair
	{
		const ColliderInfo* lhs;
		const ColliderInfo* rhs;
	};

	// ContactConstraint == CollisionPair
	struct CollisionPair { 
		Handle<Collider> lhs, rhs; 
		CollisionPair(Handle<Collider> colA, Handle<Collider> colB)
			: lhs{colA}
			, rhs{colB}
		{
			if (rhs.id < lhs.id)
				std::swap(lhs, rhs);
		}
		auto operator<=>(const CollisionPair&) const = default; 
	};

	struct pair_hasher { size_t operator()(const CollisionPair&) const; };
	using CollisionList = hash_table<CollisionPair, phys::col_success, pair_hasher>;

	constexpr auto calc_shape = [](const auto& shape, const Collider& col)
	{
		return shape * col.GetGameObject()->Transform()->GlobalMatrix();
	};
}