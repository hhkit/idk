#pragma once
#include <idk.h>
#include <stack>
#include <phys/AabbTree_utils.h>
namespace idk
{
	class AabbTree
	{
	public:
		void preallocate_nodes(size_t num);
		void add_to_update(Collider& collider, const aabb& new_bound, float margin = 0.2f);
		void update(Collider& collider, const aabb& new_bound, float margin = 0.2f);
		bool query(const aabb& rhs);
		int insert(Collider& collider, const aabb& bound, float margin);
		void remove(int index);
		void clear();
		void debug_draw() const;

		void node_deactivated(int index);
		
	private:
		
		void add_to_freelist(int index);
		int  get_node();
		int remove_node(int index);
		void balance();
		void sync(int index);

		// hash_table<Handle<Collider>, size_t> _node_table;
		vector<AabbNode> _nodes;
		// vector<update_info> _to_update;
		// std::stack<size_t> _free_list;
		int _free_list = -1;
		int _node_count = 0;
		int _root_index = -1;
		// stats
		bool _debug = true;
		int num_collision_tests = 0;
		int num_inserts = 0;
		int num_removes = 0;

		std::deque<int> collision_tests_per_second;
		std::deque<int> inserts_per_second;
		std::deque<int> removes_per_second;
	};
}