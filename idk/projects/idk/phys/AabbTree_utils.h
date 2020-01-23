#pragma once
#include <idk.h>
#include <phys/Collider.h>
#include <phys/collidable_shapes.h>

namespace idk
{
	struct AabbNode
	{
		AabbNode()
			:left{ -1 }, right{ -1 }, parent{ -1 }, height{0}
		{
		}
		bool leaf() const { return right < 0; }

		union {
			int parent;
			int next;	// free list maybe
		};
		union {
			struct {
				int left;
				int right;
			};

			Handle<Collider> collider;
		};

		aabb fat_aabb;
		int height{ 0 };
	};
}