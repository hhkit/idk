#include "stdafx.h"
#include "octree.h"
#include "Collider.h"

namespace idk
{
	
	octree::octree(vec3 center, float width, unsigned depth, float offset)
	{
		_root = std::make_shared<octree_node>();

		static const vec3 offset_dir = vec3{1/sqrtf(2.0f)};
		auto offset_vec = offset_dir * (width + offset);
		_root->bounds.min = center - offset_vec;
		_root->bounds.max = center + offset_vec;

		// TODO:
		// if(depth > 0)
	}

	void octree::insert(octree_node_info& data)
	{
		if (_root == nullptr)
		{
			_root = std::make_shared<octree_node>();

			static const vec3 offset_dir = vec3{ 1 / sqrtf(2.0f) };;
			vec3 offset_vec = offset_dir * offset;
			_root->bounds.min = data.bound.min - offset_vec;
			_root->bounds.max = data.bound.max + offset_vec;

			_root->object_list.emplace_back(data);
			return;
		}

		// Rebuild the octree if the object lies outside the octree
		if (!_root->bounds.overlaps(data.bound))
		{
			// TODO: REBUILD OCTREE
			return;
		}

		insert_data(_root, data);
	}

	void octree::erase(Handle<Collider> object)
	{
	}

	void octree::erase_from(Handle<Collider> object, shared_ptr<octree_node> node)
	{
		for (auto it = node->object_list.begin(); it < node->object_list.end(); ++it)
		{
			if (object == it->collider)
			{
				node->object_list.erase(it);
				break;
			}
		}
	}

	void octree::insert_data(shared_ptr<octree_node> node, octree_node_info& data)
	{
		
		// Simply put the object in the list if the current node has not hit the threshold for splitting
		if (node->object_list.size() + 1 <= split_threshold)
		{
			node->object_list.emplace_back(data);
			return;
		}

		// Compute the quadrant
		int quad_index = 0;
		auto half_extents = data.bound.halfextents();
		for (int i = 0; i < 3; ++i)
		{

		}
	}

	void octree::balance_tree(shared_ptr<octree_node> node)
	{
	}
}