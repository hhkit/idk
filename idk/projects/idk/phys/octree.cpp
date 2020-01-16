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

	void octree::insert(Handle<Collider> object)
	{
		// construct the data
		octree_node_data data;
		// data.object_bounds = object->bounds();
		data.bound = object->bounds();

		if (_root == nullptr)
		{
			_root = std::make_shared<octree_node>();

			static const vec3 offset_dir = vec3{ 1 / sqrtf(2.0f) };;
			vec3 offset_vec = offset_dir * offset;
			_root->bounds.min = data.bound.min - offset_vec;
			_root->bounds.max = data.bound.max + offset_vec;

			_root->object_list.emplace(object, data);
			return;
		}

		// Rebuild the octree if the object lies outside the octree
		if (!_root->bounds.overlaps(data.bound))
		{
			// TODO: REBUILD OCTREE
			return;
		}

		insert_data(_root, object, data);
	}

	void octree::insert_data(shared_ptr<octree_node> node, const Handle<Collider>& key, octree_node_data& val)
	{
		
		// Simply put the object in the list if the current node has not hit the threshold for splitting
		if (node->object_list.size() + 1 <= split_threshold)
		{
			node->object_list.emplace(key, val);
			return;
		}

		// Compute the quadrant
		int quad_index = 0;
		auto half_extents = val.bound.halfextents();
		for (int i = 0; i < 3; ++i)
		{

		}
	}

	void octree::balance_tree(shared_ptr<octree_node> node)
	{
	}
}