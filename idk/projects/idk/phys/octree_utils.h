#pragma once
#include <idk.h>
#include <core/Handle.h>
#include <phys/collidable_shapes.h>
#include <common/Layer.h>
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

	

	struct collision_info
	{
		Handle<Collider> collider;
		aabb bound;
		Octant octant;

		// Run-time
		CollidableShapes predicted_shape;
		LayerManager::layer_t layer;
	};
	struct octree_octants;

	struct octree_node
	{
		aabb bound;
		
		// objects
		hash_table<Handle<Collider>, collision_info> object_list;
		// octants
		shared_ptr<octree_node> children[8]{ nullptr };
	};

	struct octree_octants
	{
		octree_node octants[8];
	};
}