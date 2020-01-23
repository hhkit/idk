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

	static void split(shared_ptr<octree_node> node, unsigned depth)
	{
		static vec3 dir[] =
		{
			vec3{-1,-1,-1}, // 0
			vec3{-1,-1, 1}, // 1
			vec3{-1, 1,-1}, // 2
			vec3{-1, 1, 1}, // 3

			vec3{ 1,-1,-1}, // 4
			vec3{ 1,-1, 1}, // 5
			vec3{ 1, 1,-1}, // 6
			vec3{ 1, 1, 1}  // 7
		};
		if (depth == 0)
			return;

		const auto node_half_extents = node->width / 2.0f;
		const auto node_center = node->bound.center();
		for (int i = 0; i < 8; ++i)
		{
			const auto grow_vec = dir[i] * node_half_extents;

			auto new_node = std::make_shared<octree_node>();
			new_node->bound.grow(grow_vec);
			new_node->bound.center_at(node_center + grow_vec / 2.0f);
			new_node->width = node_half_extents;
			new_node->center = new_node->bound.center();

			node->children[i] = new_node;
		}

		split(node, depth - 1);
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

		split(_root, depth);

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
		// Keep object inside the root
		else if (!_root->bound.contains(data_copy.bound))
		{
			data_copy.collider->_octree_node = _root;
			_root->node_set.emplace(data_copy.collider);
			_objects.emplace(data.collider, data);
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

		auto node_center = node->center;
		auto node_half_extents = node->width / 2.0f;

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
		}

		if (!straddle)
		{
			collider_info copy = object;
			
			erase_from(object.collider, node);
			// copy.octant = s_cast<Octant>(quad_index);
			insert_data(node->children[quad_index], copy);
		}
	}

	void octree::erase(Handle<Collider> object)
	{
		if (_root)
		{
			auto res = _root->node_set.find(object);
			if (res != _root->node_set.end())
			{
				object->_octree_node.reset();

				_root->node_set.erase(res);
				_objects.erase(_objects.find(object));
			}
			else
			{
				for (auto& c_node : _root->children)
					erase_from(object, c_node);
				// res = _root->tree_set.find(object);
				// if (res != _root->tree_set.end())
				// {
				// 	_root->tree_set.erase(res);
				// 	for (auto& child : _root->children)
				// 	{
				// 		if (child)
				// 			erase_from(object, child);
				// 	}
				// }
			}
		}
	}

	void octree::erase_from(Handle<Collider> object, shared_ptr<octree_node> node)
	{
		auto res = node->node_set.find(object);
		if (res != node->node_set.end())
		{
			object->_octree_node.reset();

			node->node_set.erase(res);
			_objects.erase(_objects.find(object));
		}
		else
		{
			for (auto& c_node : node->children)
				erase_from(object, c_node);
		}
		/*else
		{
			res = node->tree_set.find(object);
			if (res != node->tree_set.end())
			{
				node->tree_set.erase(res);
				for (auto& child : node->children)
				{
					if (child)
						erase_from(object, child);
				}
			}
		}*/
	}

	void octree::erase_all()
	{
		if (_root)
		{
			// Remove all references to this node from all the objects
			for (auto& obj : _root->node_set)
				obj->_octree_node.reset();

			_root->node_set.clear();
			_root->tree_set.clear();
			for (auto& oct : _root->children)
			{
				erase_all(oct);
			}
			_objects.clear();
		}
	}

	void octree::clear()
	{
		clear(_root);
	}

	vector<collider_info> octree::get_info_by_copy(shared_ptr<octree_node> node)
	{
		vector<collider_info> info;
		info.reserve(_objects.size());
		get_info_copy(_root, info);
		return info;
	}

	vector<collider_info*> octree::get_info_by_ptr(shared_ptr<octree_node> node)
	{
		vector<collider_info*> info;
		info.reserve(_objects.size());
		get_info_ptr(node, info);
		return info;
	}

	collider_info* octree::find_subtree(Handle<Collider> object, shared_ptr<octree_node> node)
	{
		collider_info* ret_val = nullptr;
		if (node)
		{
			if (node->exists(object) || node->exists_subtree(object))
			{
				return &_objects[object];
			}
		}

		return ret_val;
	}

	collider_info* octree::find(Handle<Collider> key)
	{
		auto res = _objects.find(key);
		if (res != _objects.end())
			return &res->second;
		return nullptr;
	}

	const collider_info* octree::find(Handle<Collider> key) const
	{
		auto res = _objects.find(key);
		if (res != _objects.end())
			return &res->second;
		return nullptr;
	}

	void octree::get_info_ptr(shared_ptr<octree_node> node, vector<collider_info*>& info)
	{
		if (node)
		{
			for (auto& obj : node->node_set)
			{
				info.emplace_back(&_objects[obj]);
			}

			for (auto& c_node : node->children)
				get_info_ptr(c_node, info);

			//for (auto& obj : node->tree_set)
			//	info.emplace_back(&_objects[obj]);
		}

	}

	void octree::get_info_copy(shared_ptr<octree_node> node, vector<collider_info>& info)
	{
		if (node)
		{
			for (auto& obj : node->node_set)
			{
				info.emplace_back(_objects[obj]);
			}

			for (auto& c_node : node->children)
				get_info_copy(c_node, info);
			// for (auto& obj : node->tree_set)
			// 	info.emplace_back(_objects[obj]);
		}

	}

	void octree::insert_data(shared_ptr<octree_node> node, collider_info& data)
	{
		
		// Simply put the object in the list if the current node has not hit the threshold for splitting
		if (node->node_set.size() + 1 <= split_threshold)
		{
			data.collider->_octree_node = node;
			node->node_set.emplace(data.collider);
			_objects.emplace(data.collider, data);
			return;
		}

		for (auto& c_node : node->children)
		{
			if (c_node->bound.contains(data.bound))
			{
				// node->tree_set.emplace(data.collider);
				insert_data(c_node, data);
				return;
			}
		}

		// Insert into this node
		data.collider->_octree_node = node;
		node->node_set.emplace(data.collider);
		_objects.emplace(data.collider, data);

		// Compute the quadrant
		//bool straddle = false;
		//int quad_index = 0;
		//auto obj_half_extents = data.bound.halfextents();
		//auto obj_center = data.bound.center();

		//auto node_center = node->bound.center();
		//auto node_half_extents = node->bound.halfextents();

		//vec3 grow_dir{ 1.0f };

		//for (int i = 0; i < 3; ++i)
		//{
		//	const float delta = obj_center[i] - node_center[i];
		//	if (abs(delta) < obj_half_extents[i])// + node_half_extents[i])
		//	{
		//		straddle = true;
		//		break;
		//	}

		//	if (delta > 0.0f)
		//		quad_index |= (1 << i); // ZXY
		//	else
		//		grow_dir[i] *= -1.0f;
		//}

		//if (!straddle && node->depth < 5)
		//{
		//	// Create the child that the object will be put into
		//	if (node->children[quad_index] == nullptr)
		//	{
		//		auto new_node = std::make_shared<octree_node>();
		//		
		//		const auto grow_vec = grow_dir * node_half_extents.x;
		//		new_node->bound.grow(grow_vec);
		//		new_node->bound.center_at(node_center + grow_vec / 2.0f);
		//		new_node->depth = node->depth + 1;
		//		node->children[quad_index] = new_node;
		//	}
		//	
		//	node->tree_set.emplace(data.collider);
		//	insert_data(node->children[quad_index], data);
		//}
		//else
		//{
		//	data.collider->_octree_node = node;
		//	node->node_set.emplace(data.collider);
		//	_objects.emplace(data.collider, data);
		//}
	}

	void octree::erase_all(shared_ptr<octree_node> node)
	{
		if (node)
		{
			// Remove all references to this node from all the objects
			for (auto& obj : node->node_set)
				if(obj)
					obj->_octree_node.reset();

			node->node_set.clear();
			node->tree_set.clear();
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
			for (auto& obj : node->node_set)
			{
				if(obj)
					obj->_octree_node.reset();
			}

			node->node_set.clear();
			node->tree_set.clear();

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