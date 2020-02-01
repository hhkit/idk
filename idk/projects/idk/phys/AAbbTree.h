#pragma once
#include <idk.h>
#include <stack>
#include <phys/AabbTree_utils.h>
namespace idk
{
	class AabbTree
	{
	public:
		int find(Handle<Collider> collider) const;
		int node_count() const { return _node_count; }
		int leaf_count() const { return _leaf_count; }
		int num_tests() const { return num_collision_tests; }
		int root() const { return _root_index; }
		void debug_draw() const;
		int query_collisions(const ColliderInfo& against, vector<ColliderInfoPair>& pairs) const;

		void preallocate_nodes(size_t num);
		void update(Collider& collider, const ColliderInfo& info, float margin = 0.2f);
		int insert(Collider& collider, const ColliderInfo& bound, float margin);
		int insert(Collider& collider, const aabb& bound, float margin);
		bool remove(int index);
		void clear();
		
		void reset_stats();
		friend class PhysicsSystem;
	private:
		
		void add_to_freelist(int index);
		int  get_node();
		void sync(int index);

		hash_table<Handle<Collider>, int> _node_table;
		vector<AabbNode> _nodes;
		
		int _free_list = -1;
		int _node_count = 0;
		int _leaf_count = 0;
		int _root_index = -1;

		// stats (not used yet)
		bool _debug = true;
		mutable int num_collision_tests = 0;
		mutable int num_inserts = 0;
		mutable int num_removes = 0;

		std::deque<int> collision_tests_per_second;
		std::deque<int> inserts_per_second;
		std::deque<int> removes_per_second;
		
	};
}