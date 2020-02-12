#include <stdafx.h>
#include <phys/AAbbTree.h>
#include <phys/raycasts/collision_raycast.h>
#include <gfx/DebugRenderer.h>
#include <core/GameObject.h>
#include <phys/PhysicsSystem.h>
#include <math/shapes/ray.h>
#include <ds/result.inl>
#include <stack>
namespace idk
{
#define AREA_BALANCED 1
	void AabbTree::preallocate_nodes(size_t num)
	{
		_node_table.clear();
		_nodes.clear();
		_nodes.resize(num);
		// Clear doesnt call _nodes.clear(). Instead, it adds all the nodes in _nodes to the free_list
		clear();
	}

	void AabbTree::update(Collider& collider, const ColliderInfo& info, float margin)
	{
		auto& node = _nodes[collider.node_id];
		int index = collider.node_id;
		if (!node.fat_aabb.contains(info.broad_phase))
		{
			if (remove(collider.node_id))
			{
				collider.node_id = -1;
				index = insert(collider, info, margin);
			}
		}

		_nodes[index].info = info;
	}

	int AabbTree::insert(Collider& collider, const ColliderInfo& info, float margin)
	{
		const auto index = insert(collider, info.broad_phase, margin);
		if (index >= 0)
		{
			_nodes[index].info = info;
		}
		return index;
	}

	int AabbTree::find(Handle<Collider> collider) const
	{
		auto res = _node_table.find(collider);
		if (res == _node_table.end())
			return -1;
		return res->second;
	}

	int AabbTree::query_collisions(const ColliderInfo& against, vector<ColliderInfoPair>& pairs) const
	{
		if (_root_index < 0)
			return 0;

		const auto& phys_sys = Core::GetSystem<PhysicsSystem>();
		int num_collisions = 0;
		int sp = 1;
		constexpr int max_size_stack = 4096;
		int stack[max_size_stack];

		*stack = _root_index;
		while (sp)
		{
			const int index = stack[--sp];
			const auto& node = _nodes[index];
			
			// Check if the bounds of the leaf node collides against the other bounds
			if (node.leaf())
			{
				if (!phys_sys.AreLayersCollidable(against.layer, node.info.layer))
					continue;

				++num_collision_tests;
				if (!against.broad_phase.overlaps(node.info.broad_phase))
					continue;

				pairs.emplace_back(ColliderInfoPair{ &against,&node.info });
				++num_collisions;
			}
			else
			{
				if (node.fat_aabb.overlaps(against.broad_phase))
				{
					stack[sp++] = node.left;
					stack[sp++] = node.right;
				}
				++num_collision_tests;
			}
		}
		
		return num_collisions;
	}
	vector<RaycastHit> AabbTree::query_raycast(const ray& raycast, LayerMask mask)
	{
		if (_root_index < 0)
			return {};
		vector<RaycastHit> hitted;
		const auto& phys_sys = Core::GetSystem<PhysicsSystem>();
		int num_collisions = 0;
		int sp = 1;
		constexpr int max_size_stack = 4096;
		int stack[max_size_stack];

		*stack = _root_index;
		while (sp)
		{
			const int index = stack[--sp];
			const auto& node = _nodes[index];

			// Check if the bounds of the leaf node collides against the other bounds
			if (node.leaf())
			{
				auto layer = node.info.layer;
				if (!(mask & LayerMask{ 1 << layer }))
					continue;

				++num_collision_tests;
				if (auto res = phys::collide_ray_aabb(raycast, node.info.broad_phase))
				{
					hitted.emplace_back(RaycastHit{.collider = node.collider, .raycast_succ = *res});
					++num_collisions;
					continue;
				}
				else
					continue;
			}
			else
			{
				if (phys::collide_ray_aabb(raycast, node.fat_aabb))
				{
					stack[sp++] = node.left;
					stack[sp++] = node.right;
				}
				++num_collision_tests;
			}
		}

		return hitted;
	}
// 
	int AabbTree::insert(Collider& collider, const aabb& bound, float margin)
	{
		auto res = _node_table.find(collider.GetHandle());
		if (res != _node_table.end())
		{
			return collider.node_id;
		}

		int ret_val = -1;

		// Initialize the new node with data
		const vec3 grow_vec{ margin };
		auto inserted_aabb = bound;
		inserted_aabb.min -= grow_vec;
		inserted_aabb.max += grow_vec;

		// First node
		if (_node_count == 0)
		{
			// Create first node and add to the stack
			int inserted_index = get_node();
			auto& inserted_node = _nodes[inserted_index];
			inserted_node.collider = collider.GetHandle();
			inserted_node.fat_aabb = inserted_aabb;
			inserted_node.info.broad_phase = bound;
			// inserted_node.info.layer = collider.GetGameObject()->Layer();
			collider.node_id = inserted_index;

			_root_index = inserted_index;
			++_leaf_count;

			_node_table.emplace(collider.GetHandle(), inserted_index);
			return inserted_index;
		}

		int to_explore = _root_index;
		int parent_index = -1;		
		aabb parent_combined = _nodes[_root_index].fat_aabb.combine(inserted_aabb);

		while (to_explore >= 0)
		{
			if (_nodes[to_explore].leaf())
			{
				// Create parent and new node
				int inserted_index = get_node();
				parent_index = get_node();
				auto& inserted_node = _nodes[inserted_index];
				auto& parent_node = _nodes[parent_index];
				auto& node = _nodes[to_explore];

				// Initializing the node to be inserted
				inserted_node.collider = collider.GetHandle();
				inserted_node.fat_aabb = inserted_aabb;
				inserted_node.parent = parent_index;
				collider.node_id = inserted_index;
				
				// Initializing the parent node
				parent_node.parent = node.parent;
				parent_node.left = to_explore;
				parent_node.right = inserted_index;

				// Link current leaf node to the parent as well
				node.parent = parent_index;

				// Inserting into root
				if (to_explore == _root_index)
				{
					_root_index = parent_index;
				}
				else if (parent_node.parent >= 0)
				{
					// Grandparent's links need to change to link to the new parent
					auto& gp_node = _nodes[parent_node.parent];
					gp_node.left == to_explore ? gp_node.left = parent_index : gp_node.right = parent_index;
				}

#if AREA_BALANCED
				// Need to compute parents size
				parent_node.fat_aabb = parent_combined;
#else
				// No need to update the aabb of this parent node -> We do it when we call sync anyways.
#endif
				++_leaf_count;
				ret_val = inserted_index;
				break;
			}
			
			auto& node = _nodes[to_explore];
			auto& node_left = _nodes[node.left];
			auto& node_right = _nodes[node.right];

#if AREA_BALANCED
			node.fat_aabb = parent_combined;

			const auto l_pre_vol = node_left.fat_aabb.volume();
			const auto l_combined = node_left.fat_aabb.combine(inserted_aabb);
			const auto l_delta = l_combined.volume() - l_pre_vol;

			const auto r_pre_vol = node_right.fat_aabb.volume();
			const auto r_combined = node_right.fat_aabb.combine(inserted_aabb);
			const auto r_delta = r_combined.volume() - r_pre_vol;

			if (l_delta < r_delta)
			{
				// Insert into left since there is smaller volume change
				// Make sure to update the aabb when inserting into left side
				// ++node_left.height;
				parent_combined = l_combined;
				to_explore = node.left;
			}
			else
			{
				// Insert into right since there is smaller volume change
				// Make sure to update the aabb when inserting into right side
				// ++node_right.height;
				parent_combined = r_combined;
				to_explore = node.right;
			}
#else
			to_explore = node_left.height <= node_right.height ? node.left : node.right
#endif
			
			
		}
#if AREA_BALANCED
		// Make sure the root node encloses the new node
		// _nodes[_root_index].fat_aabb.surround(inserted_node.fat_aabb);
#else
		sync(parent_index);
#endif
		IDK_ASSERT(parent_index >= 0);
		IDK_ASSERT(ret_val >= 0);

		_node_table.emplace(collider.GetHandle(), ret_val);
		return ret_val;
	}

	bool AabbTree::remove(int index)
	{
		if (index < 0 || index >= _nodes.size())
		{
			LOG_CRASH_TO(LogPool::PHYS, "Removing element that doesnt exist");
			return false;
		}

		auto& node_to_remove = _nodes[index];
		auto res = _node_table.find(node_to_remove.collider);
		if (res == _node_table.end())
			return false;

		// Must be valid and leaf node
		if (!node_to_remove.valid || !node_to_remove.leaf())
			return false;

		// Removing only node in the tree
		if (index == _root_index)
		{
			add_to_freelist(index);
			_root_index = -1;
			_node_count = 0;
			_leaf_count = 0;

			_node_table.erase(res);
			return true;
		}

		int parent_index = node_to_remove.parent;
		auto& parent_node = _nodes[parent_index];

		const auto remove_impl = [&](int sibling_index)
		{
			const auto gp_index = parent_node.parent;
			auto& sibling = _nodes[sibling_index];
			auto& grandparent_node = _nodes[gp_index];

			// Remove the parent's connection to its grandparent and link the sibling to the grandparent
			grandparent_node.left == parent_index ? grandparent_node.left = sibling_index : grandparent_node.right = sibling_index;

			// Make sibling's parent the grandparent
			sibling.parent = parent_node.parent;

			add_to_freelist(parent_index);
			add_to_freelist(index);
			// node_to_remove.collider->node_id = -1;
			
			sync(sibling.parent);
			// Means node's parent is root
		};

		const auto sibling_index = parent_node.left == index ? parent_node.right : parent_node.left;
		if (parent_index == _root_index)
		{
			_root_index = sibling_index;
			_nodes[_root_index].parent = -1;
			add_to_freelist(parent_index);
			add_to_freelist(index);
		}
		else
		{
			remove_impl(sibling_index);
		}

		_node_table.erase(res);
		--_leaf_count;
		return true;
	}

	void AabbTree::clear()
	{
		const int max_size = s_cast<int>(_nodes.size());
		_free_list = -1;

		// Insert backwards
		for (int i = max_size - 1; i >= 0; --i)
		{
			auto& node = _nodes[i];
			if (node.collider)
				node.collider->node_id = -1;
			add_to_freelist(i);
		}
		// IDK_ASSERT(_free_list == 0);
		_node_count = 0;
		_leaf_count = 0;
		_root_index = -1;
		_node_table.clear();
	}

	void AabbTree::reset_stats()
	{
		num_collision_tests = 0;
		num_inserts = 0;
		num_removes = 0;
	}

	void AabbTree::debug_draw() const
	{
		for (auto& node : _nodes)
		{
			if (node.valid && node.leaf())
			{
				std::visit([&](const auto& shape)
					{
						Core::GetSystem<DebugRenderer>().Draw(shape, node.collider->is_trigger ? color{ 0, 1, 1 } : color{ 1, 0, 0 }, Core::GetDT());
					}, node.info.predicted_shape);
				// color col = node.leaf() ? color{ 0,1,0,1 } : color{ 1,1,1,0 };
				// Core::GetSystem<DebugRenderer>().Draw(node.fat_aabb, col, seconds{1.0f / 60.0f}, false);
			}
		}
	}

	void AabbTree::add_to_freelist(int index)
	{
		auto& node = _nodes[index];
		
		node.parent = -1;
		node.next = -1;
		node.left = -1;
		node.right = -1;
		node.valid = false;
		node.collider = Handle<Collider>{};

		// First node in the free list
		if (_free_list < 0)
		{
			_free_list = index;
		}
		else
		{
			_nodes[index].next = _free_list;
			_free_list = index;
		}

		--_node_count;
	}

	int AabbTree::get_node()
	{
		auto ret_val = -1;
		if (_free_list >= 0)
		{
			ret_val = _free_list;
			auto& node = _nodes[ret_val];
			_free_list = node.next;
			node.valid = true;
			node.parent = -1;
		}
		else
		{
			AabbNode new_node;
			new_node.valid = true;
			ret_val = s_cast<int>(_nodes.size());
			_nodes.emplace_back(new_node);
			
		}

		++_node_count;
		return ret_val;
	}

	void AabbTree::sync(int index)
	{
#if AREA_BALANCED
		while (index >= 0.0f)
		{
			auto& node = _nodes[index];
			auto& left_node = _nodes[node.left];
			auto& right_node = _nodes[node.right];

			// Balance the tree here first

			// We dont care about height if the its area balanced
			// node.height = 1 + max(left_node.height, right_node.height);
			node.fat_aabb = left_node.fat_aabb.combine(right_node.fat_aabb);

			index = node.parent;
		}
#else
		while (index >= 0)
		{
			auto& node = _nodes[index];
			auto& left_node = _nodes[node.left];
			auto& right_node = _nodes[node.right];

			// Balance the tree here first

			node.height = 1 + max(left_node.height, right_node.height);
			node.fat_aabb = left_node.fat_aabb.combine(right_node.fat_aabb);

			index = node.parent;
		}
#endif
	}
}