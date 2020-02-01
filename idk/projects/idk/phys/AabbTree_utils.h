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

	struct AabbNode
	{
		AabbNode() : parent{ -1 } {}

		bool leaf() const { return valid && left < 0; }
		// bool valid() const { return }
		int parent = -1;
		int next = -1;	// free list maybe
		
		int left = -1;
		int right = -1;
		Handle<Collider> collider;
		aabb fat_aabb;
		int height{ 0 };
		bool valid = false;

		// Run-time stuff
		ColliderInfo info;
	};
}