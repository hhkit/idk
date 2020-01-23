#pragma once
#include <idk.h>
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
		bool is_in_subtree(shared_ptr<octree_node> node, Handle<Collider> object);
		vector<collider_info> get_info_by_copy(shared_ptr<octree_node> node);
		vector<collider_info*> get_info_by_ptr(shared_ptr<octree_node> node);
		collider_info* search_tree(Handle<Collider> object, shared_ptr<octree_node> node);

		// Rebuilds the octree with this object inserted
		// void rebuild(float offset = 2.0f, octree_obj obj = octree_obj{});
		
		void rebuild();
		void rebuild(vec3 center, float width, unsigned depth, float offset = 2.0f);
		void insert(const collider_info& object);
		void descend(shared_ptr<octree_node> node, const collider_info& object);
		void erase(Handle<Collider> object);
		// Removes the object from the node
		void erase_from(Handle<Collider> object, shared_ptr<octree_node> node);
		void erase_all();
		
		
		void clear();
	private:
		
		size_t object_count = 0;
		shared_ptr<octree_node> _root{};
		friend PhysicsSystem;

		
		void get_info_copy(shared_ptr<octree_node> node, vector<collider_info>& info);
		void get_info_ptr(shared_ptr<octree_node> node, vector<collider_info*>& info);
		
		void insert_data(shared_ptr<octree_node> node, collider_info& data);
		void erase_all(shared_ptr<octree_node> node);
		void clear(shared_ptr<octree_node> node);
		void balance_tree(shared_ptr<octree_node> node);
	};
}