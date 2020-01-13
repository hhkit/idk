#include "stdafx.h"
#include "octree.h"
#include "Collider.h"

namespace idk
{
	void octree_node::split()
	{
		// split into 8 child nodes and re-insert all objects
	}

	octree::octree(vec3 center, float width, unsigned depth)
	{

	}

	void octree::insert(octree_obj object)
	{
		// construct the data
		octree_node_data data;
		data.object_bounds = object->bounds();
		data.object = object;

		if (_root == nullptr)
		{
			_root = std::make_shared<octree_node>();

			return;
		}

		insert_data(_root, object, data);
	}

	void octree::insert_data(shared_ptr<octree_node> node, const octree_obj& key, octree_node_data& val)
	{
		// Simply put the object in the list if the current node has not hit the threshold for splitting
		if (node->object_list.size() + 1 <= octree_node::SPLIT_THRESHOLD)
		{
			node->object_list.emplace(key, val);
			return;
		}

		// Compute the quadrant
		int quad_index = 0;
		auto half_extents = val.object_bounds.halfextents();
		for (int i = 0; i < 3; ++i)
		{

		}
	}
}