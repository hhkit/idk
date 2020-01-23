#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <phys/collidable_shapes.h>
#include <common/LayerManager.h>
namespace idk
{
	enum Octant
	{
		NONE = -1,	//				(ZYX)
		LBN = 0,	// -x, -y, -z	(000)0
		LBF,		//  x, -y, -z	(001)1

		LTN,		// -x,  y, -z	(010)2
		LTF,		//  x,  y, -z	(011)3

		RBN,		// -x, -y,  z	(100)4
		RBF,		//  x, -y,  z	(101)5

		RTN,		//  x,  y,  z	(110)6
		RTF,		//  x,  y,  z	(111)7

		MAX
	};

	

	struct collider_info
	{
		Handle<Collider> collider;
		aabb bound;

		// Run-time
		CollidableShapes predicted_shape;
		LayerManager::layer_t layer;
	};
	struct octree_octants;

	struct octree_node
	{
		aabb bound;
		float width{ 0.0f };
		vec3 center{0.0f};

		// objects
		hash_set<Handle<Collider>> node_set;
		hash_set<Handle<Collider>> tree_set;
		// octants
		shared_ptr<octree_node> children[8]{ nullptr };
		size_t depth{ 0 };

		bool exists(Handle<Collider> obj) const { return node_set.find(obj) != node_set.end(); }
		bool exists_subtree(Handle<Collider> obj) const { return tree_set.find(obj) != tree_set.end(); }
		bool contains(const collider_info& col_info) const { return bound.contains(col_info.bound); }
		size_t size() const { return node_set.size(); }
	};

	struct octree_octants
	{
		octree_node octants[8];
	};
}