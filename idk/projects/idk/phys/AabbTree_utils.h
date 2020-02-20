#pragma once
#include <idk.h>
#include <phys/Collision_utils.h>

namespace idk
{
	

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