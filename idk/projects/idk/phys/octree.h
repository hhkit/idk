#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <phys/collidable_shapes.h>
#include <phys/octree_utils.h>

namespace idk
{
	

	struct octree
	{
	public:
		float offset{ 2.0f };
		size_t split_threshold{ 1 };

		octree() = default;

		// Preallocate the tree
		octree(vec3 center, float width, unsigned depth, float offset = 2.0f);

		aabb bounds() const { return _root->bounds; }

		// Rebuilds the octree with this object inserted
		// void rebuild(float offset = 2.0f, octree_obj obj = octree_obj{});
		void balance_tree();
		void insert(Handle<Collider> object);
		void erase(Handle<Collider> object);
		// Removes the object from the node
		void erase_from(Handle<Collider> object, shared_ptr<octree_node> node);
		
	private:
		shared_ptr<octree_node> _root{};

		void insert_data(shared_ptr<octree_node> node, const Handle<Collider>& key, octree_node_data& data);
		void balance_tree(shared_ptr<octree_node> node);
	};
}