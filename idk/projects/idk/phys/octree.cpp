#include "stdafx.h"
#include "octree.h"
#include "Collider.h"

namespace idk
{
	
	octree::octree(vec3 center, float width, unsigned depth, float offset)
	{
		rebuild(center, width, depth, offset);
		_root = std::make_shared<octree_node>();

		static const vec3 offset_dir = vec3{ phys::utils::inv_sqrt_3 };;
		const auto offset_vec = offset_dir * (width + offset);
		_root->bound.min = center - offset_vec;
		_root->bound.max = center + offset_vec;

		// TODO:
		// if(depth > 0)
	}

	void octree::rebuild()
	{
		auto info = get_all_info(_root);
		clear();
		
		// Re-insert all objects
		for (auto& obj : info)
			insert(obj);
	}

	void octree::rebuild(vec3 center, float width, unsigned depth, float offset)
	{
		auto info = get_all_info(_root);
		clear();

		this->offset = offset;
		_root->bound.min = center - vec3{phys::utils::inv_sqrt_3} * (width + offset);
		_root->bound.max = center + vec3{phys::utils::inv_sqrt_3} * (width + offset);

		// Re-insert all objects
		for (auto& obj : info)
			insert(obj);
	}

	void octree::insert(collision_info& data)
	{
		if (_root == nullptr)
		{
			_root = std::make_shared<octree_node>();
			
			static const vec3 offset_dir{  phys::utils::inv_sqrt_3 };
			const vec3 offset_vec = offset_dir * offset;
			_root->bound.min = data.bound.min - offset_vec;
			_root->bound.max = data.bound.max + offset_vec;

			_root->object_list.emplace(data.collider, data);
			data.collider->_octree_node = _root;
			return;
		}

		// Rebuild the octree if the object lies outside the octree
		if (!_root->bound.overlaps(data.bound))
		{
			auto new_bound = _root->bound;
			new_bound.surround(data.bound);
			auto new_extents = new_bound.extents();

			auto width = max(max(new_extents.x, new_extents.y), new_extents.z);
			rebuild(new_bound.center(), width, 0);
			return;
		}

		insert_data(_root, data);
	}

	void octree::erase(Handle<Collider> object)
	{
	}

	void octree::erase_from(Handle<Collider> object, shared_ptr<octree_node> node)
	{
		if (!node)
			return;

		auto res = node->object_list.find(object);
		if (res != node->object_list.end())
		{
			object->_octree_node.reset();
			node->object_list.erase(res);
		}
	}

	void octree::erase_all()
	{
		if (_root)
		{
			// Remove all references to this node from all the objects
			for (auto& obj : _root->object_list)
				obj.second.collider->_octree_node.reset();

			_root->object_list.clear();
			for (auto& oct : _root->children)
			{
				erase_all(oct);
			}
		}
	}

	void octree::clear()
	{
		clear(_root);
	}

	vector<collision_info> octree::get_all_info(shared_ptr<octree_node> node)
	{
		vector<collision_info> info;
		info.reserve(object_count);
		get_all_info(_root, info);
		return info;
	}

	void octree::get_all_info(shared_ptr<octree_node> node, vector<collision_info>& info)
	{
		if (node)
		{
			for (auto& obj : node->object_list)
				info.push_back(obj.second);

			for (auto& child : node->children)
				get_all_info(child, info);
		}

	}

	void octree::insert_data(shared_ptr<octree_node> node, collision_info& data)
	{
		
		// Simply put the object in the list if the current node has not hit the threshold for splitting
		if (node->object_list.size() + 1 <= split_threshold)
		{
			node->object_list.emplace(data.collider, data);
			return;
		}

		// Compute the quadrant
		bool straddle = false;
		int quad_index = 0;
		auto obj_half_extents = data.bound.halfextents();
		auto obj_center = data.bound.center();

		auto node_center = node->bound.center();
		auto node_half_extents = node->bound.halfextents();

		vec3 grow_dir{ phys::utils::inv_sqrt_3 };

		for (int i = 0; i < 3; ++i)
		{
			const float delta = obj_center[i] - node_center[i];
			if (abs(delta) < obj_half_extents[i] + node_half_extents[i])
			{
				straddle = true;
				break;
			}

			if (delta < 0.0f)
				quad_index |= (1 << i);
			else
				grow_dir[i] *= -1.0f;
		}

		

		if (!straddle)
		{
			// Create the child that the object will be put into
			if (node->children[quad_index] == nullptr)
			{
				node->children[quad_index] = std::make_shared<octree_node>();
				node->children[quad_index]->bound.grow(node_half_extents);
				node->children[quad_index]->bound.center_at(node_center + grow_dir * node_half_extents);
			}

			data.octant = s_cast<Octant>(quad_index);
			insert_data(node->children[quad_index], data);
		}
		else
		{
			data.collider->_octree_node = node;
			node->object_list.emplace(data.collider, data);
		}
	}

	void octree::erase_all(shared_ptr<octree_node> node)
	{
		if (node)
		{
			// Remove all references to this node from all the objects
			for (auto& obj : node->object_list)
				obj.second.collider->_octree_node.reset();

			node->object_list.clear();
			for (auto& oct : node->children)
			{
				erase_all(oct);
			}
		}
	}

	void octree::clear(shared_ptr<octree_node> node)
	{
		if (node)
		{
			// Remove all references to this node from all the objects
			for (auto& obj : node->object_list)
				obj.second.collider->_octree_node.reset();

			node->object_list.clear();

			for (auto& oct : node->children)
			{
				if (oct)
				{
					clear(oct);
					oct.reset();

					// Should not have any references to oct anymore
					IDK_ASSERT(oct == nullptr);
				}
			}
		}
	}

	void octree::balance_tree(shared_ptr<octree_node> node)
	{
	}
}