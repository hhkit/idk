#include <stdafx.h>
#include <phys/AAbbTree.h>

namespace idk
{
	void AabbTree::preallocate_nodes(size_t num)
	{
		// _node_table.clear();
		_nodes.clear();

		while(!_free_list.empty())
			_free_list.pop();

		_nodes.reserve(num);
	}

	int AabbTree::insert(Handle<Collider> collider, const aabb& bound, float margin)
	{
		const auto init_new_node = [&](AabbNode& node, int parent_index)
		{
			node.collider = collider;
			node.fat_aabb = bound;
			node.fat_aabb.grow(vec3{ margin });
			node.parent = parent_index;
		};

		

		std::stack<int> to_explore;
		// First node
		if (_nodes.empty())
		{
			// Create first node and add to the stack
			AabbNode node;
			init_new_node(node, -1);
			
			// _node_table.emplace(collider, 0);
			_nodes.emplace_back(node);
			return;
		}
		else
			to_explore.push(0);

		int parent_index = -1;
		int inserted_index = -1;
		while (!to_explore.empty())
		{
			const auto index = to_explore.top();
			to_explore.pop();
			auto& node = _nodes[index];
			if (node.leaf())
			{
				inserted_index = get_node();
				auto left_node = _nodes[inserted_index];
				init_new_node(left_node, index);

				auto right_node = node;

				node.fat_aabb = left_node.fat_aabb;
				node.fat_aabb.surround(right_node.fat_aabb);
				break;
			}
			
			const auto& node_left = _nodes[node.left];
			const auto& node_right = _nodes[node.right];
			to_explore.push(node_left.height <= node_right.height ? node.left : node.right);
		}

		sync(parent_index);
		return inserted_index;
	}

	void AabbTree::remove(int index)
	{
	}

	void AabbTree::debug_draw() const
	{
	}

	int AabbTree::get_node()
	{
		
	}

	void AabbTree::sync(int index)
	{
		while (index >= 0)
		{
			auto& node = _nodes[index];
			auto& left_node = _nodes[node.left];
			auto& right_node = _nodes[node.right];

			node.height = 1 + max(left_node.height, right_node.height);
			auto left_bound = left_node.fat_aabb;
			node.fat_aabb = left_bound.surround(right_node.fat_aabb);
			// node.fat_aabb.grow(vec3{})

			index = node.parent;
		}
	}
}