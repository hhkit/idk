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
		void update();
		bool query(const aabb& rhs);
		int insert(Handle<Collider> collider, const aabb& bound, float margin);
		void remove(int index);
		void debug_draw() const;

	private:
		int get_node();
		void balance();
		void sync(int index);

		// hash_table<Handle<Collider>, size_t> _node_table;
		vector<AabbNode> _nodes;
		std::stack<size_t> _free_list;
	};
}