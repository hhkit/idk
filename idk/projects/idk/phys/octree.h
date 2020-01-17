#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <phys/collidable_shapes.h>
#include <phys/octree_utils.h>

namespace idk
{
	

	class octree
	{
	public:
		float offset{ 2.0f };
		size_t split_threshold{ 1 };

		octree() = default;

		// Preallocate the tree
		octree(vec3 center, float width, unsigned depth, float offset = 2.0f);

		aabb bounds() const { return _root->bound; }

		// Rebuilds the octree with this object inserted
		// void rebuild(float offset = 2.0f, octree_obj obj = octree_obj{});
		void rebuild();
		void rebuild(vec3 center, float width, unsigned depth, float offset = 2.0f);
		void insert(collision_info& object);
		void erase(Handle<Collider> object);
		// Removes the object from the node
		void erase_from(Handle<Collider> object, shared_ptr<octree_node> node);
		void erase_all();
		
		void clear();
	private:
		size_t object_count = 0;
		shared_ptr<octree_node> _root{};

		vector<collision_info> get_all_info(shared_ptr<octree_node> node);
		void get_all_info(shared_ptr<octree_node> node, vector<collision_info>& info);
		void insert_data(shared_ptr<octree_node> node, collision_info& data);
		void erase_all(shared_ptr<octree_node> node);
		void clear(shared_ptr<octree_node> node);
		void balance_tree(shared_ptr<octree_node> node);
	};
}