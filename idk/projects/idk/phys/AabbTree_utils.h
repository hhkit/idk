#pragma once
#include <idk.h>
#include <phys/Collider.h>
#include <phys/collidable_shapes.h>
#include <common/LayerManager.h>

namespace idk
{
	struct update_info
	{
		Collider& collider;
		aabb new_bound;
		float margin = 0.2f;
		
	};

	struct AabbNode
	{
		AabbNode() : parent{ -1 } {}

		bool leaf() const { return valid && left < 0; }
		// bool valid() const { return }
		union {
			int parent;
			int next;	// free list maybe
		};
		
		int left = -1;
		int right = -1;
		Handle<Collider> collider;
		aabb fat_aabb;
		int height{ 0 };
		bool valid = false;

		// Run-time stuff
		LayerManager::layer_t layer;
	};
}