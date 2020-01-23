#include "stdafx.h"
#include "octree.h"
#include "Collider.h"

namespace idk
{
	
	octree::octree(vec3 center, float width, unsigned depth, float offset)
	{
		rebuild(center, width, depth, offset);
		_root = std::make_shared<octree_node>();

		// static const vec3 offset_dir = vec3{ phys::utils::inv_sqrt_3 };;
		const vec3 offset_vec{ (width + offset) /2.0f };
		_root->bound.min = center - offset_vec;
		_root->bound.max = center + offset_vec;

		// TODO:
		// if(depth > 0)
	}

	bool octree::is_in_subtree(shared_ptr<octree_node> node, Handle<Collider> object)
	{
		return false;
	}

	void octree::rebuild()
	{
		auto info = get_info_by_copy(_root);
		clear();

		if (_root == nullptr)
		{
			_root = std::make_shared<octree_node>();
		}
		
		// Re-insert all objects
		for (auto& obj : info)
			insert(obj);
	}

	void octree::rebuild(vec3 center, float width, unsigned depth, float off)
	{
		depth;
		auto info = get_info_by_copy(_root);
		clear();

		if (_root == nullptr)
		{
			_root = std::make_shared<octree_node>();
		}
		this->offset = off;
		auto half_extents = (width + offset) / 2;
		_root->bound.min = center - vec3{ half_extents } ;
		_root->bound.max = center + vec3{ half_extents };

		// Re-insert all objects
		for (auto& obj : info)
			insert(obj);
	}

	void octree::insert(const collider_info& data)
	{
		auto data_copy = data;
		if (_root == nullptr)
		{
			_root = std::make_shared<octree_node>();
			
			// static const vec3 offset_dir{  phys::utils::inv_sqrt_3 };
			const vec3 offset_vec{ offset / 2.0f };
			_root->bound.min = data.bound.min - offset_vec;
			_root->bound.max = data.bound.max + offset_vec;

			// data_copy.collider->_octree_node = _root;
			// _root->object_list.emplace(data_copy.collider, data_copy);
			// 
			// return;
		}
		// Rebuild the octree if the object lies outside the octree
		else if (!_root->bound.contains(data_copy.bound))
		{
			auto new_bound = _root->bound;
			new_bound.surround(data_copy.bound);
			auto new_extents = new_bound.extents();

			auto width = max(max(new_extents.x, new_extents.y), new_extents.z);
			rebuild(new_bound.center(), width, 0);
			// return;
		}

		insert_data(_root, data_copy);
	}

	void octree::descend(shared_ptr<octree_node> node, const collider_info& object)
	{
		// Compute the quadrant
		bool straddle = false;
		int quad_index = 0;
		auto obj_half_extents = object.bound.halfextents();
		auto obj_center = object.bound.center();

		auto node_center = node->bound.center();
		auto node_half_extents = node->bound.halfextents();

		vec3 grow_dir{ 1.0f };

		for (int i = 0; i < 3; ++i)
		{
			const float delta = obj_center[i] - node_center[i];
			if (abs(delta) < abs(obj_half_extents[i]))// + node_half_extents[i])
			{
				straddle = true;
				return;
			}

			if (delta > 0.0f)
				quad_index |= (1 << i); // ZXY
			else
				grow_dir[i] *= -1.0f;
		}

		if (!straddle)
		{
			collider_info copy = object;

			// Create the child that the object will be put into
			if (node->children[quad_index] == nullptr)
			{
				node->children[quad_index] = std::make_shared<octree_node>();
				const auto grow_vec = grow_dir * abs(node_half_extents.x);
				node->children[quad_index]->bound.grow(grow_vec);
				node->children[quad_index]->bound.center_at(node_center + grow_vec / 2.0f);
			}
			erase_from(object.collider, node);
			// copy.octant = s_cast<Octant>(quad_index);
			insert_data(node->children[quad_index], copy);
		}
	}

	void octree::erase(Handle<Collider> object)
	{
		object;
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
			object_count = object_count ?  object_count - 1 : 0;
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
			object_count = 0;
		}
	}

	void octree::clear()
	{
		clear(_root);
	}

	vector<collider_info> octree::get_info_by_copy(shared_ptr<octree_node> node)
	{
		vector<collider_info> info;
		info.reserve(object_count);
		get_info_copy(_root, info);
		return info;
	}

	vector<collider_info*> octree::get_info_by_ptr(shared_ptr<octree_node> node)
	{
		vector<collider_info*> info;
		info.reserve(object_count);
		get_info_ptr(node, info);
		return info;
	}

	collider_info* octree::search_tree(Handle<Collider> object, shared_ptr<octree_node> node)
	{
		collider_info* ret_val = nullptr;
		if (node)
		{
			auto res = node->object_list.find(object);
			if(res != node->object_list.end())
				ret_val = &res->second;
			else
			{
				// Search subtree and break when a match is found
				for (auto& child : node->children)
				{
					ret_val = search_tree(object, child);
					if (ret_val)
						break;
				}
			}
		}

		return ret_val;
	}

	void octree::get_info_ptr(shared_ptr<octree_node> node, vector<collider_info*>& info)
	{
		if (node)
		{
			for (auto& obj : node->object_list)
				info.emplace_back(&obj.second);

			for (auto& child : node->children)
				get_info_ptr(child, info);
		}

	}

	void octree::get_info_copy(shared_ptr<octree_node> node, vector<collider_info>& info)
	{
		if (node)
		{
			for (auto& obj : node->object_list)
				info.emplace_back(obj.second);

			for (auto& child : node->children)
				get_info_copy(child, info);
		}

	}

	void octree::insert_data(shared_ptr<octree_node> node, collider_info& data)
	{
		
		// Simply put the object in the list if the current node has not hit the threshold for splitting
		if (node->object_list.size() + 1 <= split_threshold)
		{
			data.collider->_octree_node = node;
			node->object_list.emplace(data.collider, data);
			++object_count;
			return;
		}

		// Compute the quadrant
		bool straddle = false;
		int quad_index = 0;
		auto obj_half_extents = data.bound.halfextents();
		auto obj_center = data.bound.center();

		auto node_center = node->bound.center();
		auto node_half_extents = node->bound.halfextents();

		vec3 grow_dir{ 1.0f };

		for (int i = 0; i < 3; ++i)
		{
			const float delta = obj_center[i] - node_center[i];
			if (abs(delta) < obj_half_extents[i])// + node_half_extents[i])
			{
				straddle = true;
				break;
			}

			if (delta > 0.0f)
				quad_index |= (1 << i); // ZXY
			else
				grow_dir[i] *= -1.0f;
		}

		if (!straddle)
		{
			// Create the child that the object will be put into
			if (node->children[quad_index] == nullptr)
			{
				auto new_node = std::make_shared<octree_node>();
				
				const auto grow_vec = grow_dir * node_half_extents.x;
				new_node->bound.grow(grow_vec);
				new_node->bound.center_at(node_center + grow_vec / 2.0f);
				new_node->depth = node->depth + 1;
				node->children[quad_index] = new_node;
			}
			// data.octant = s_cast<Octant>(quad_index);
			insert_data(node->children[quad_index], data);
		}
		else
		{
			data.collider->_octree_node = node;
			node->object_list.emplace(data.collider, data);
			++object_count;
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