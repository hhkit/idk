#pragma once
#include <core/Handle.h>
#include <phys/collidable_shapes.h>

namespace idk
{
	enum Octant
	{
		NONE = -1,
		LBN = 0,	// -x, -y, -z	(000)0
		LBF,		// -x, -y,  z	(001)1

		LTN,		// -x,  y, -z	(010)2
		LTF,		// -x,  y,  z	(011)3

		RBN,		//  x, -y, -z	(100)4
		RBF,		//  x, -y,  z	(101)5

		RTN,		//  x,  y, -z	(110)6
		RTF,		//  x,  y,  z	(111)7

		MAX
	};

	struct octree_node_info
	{
		Handle<Collider> collider;
		aabb bound;
		Octant octant;
	};

	struct octree_node
	{
		aabb bounds;

		// objects
		vector<octree_node_info> object_list;
		// octants
		shared_ptr<octree_node> children[8]{ nullptr };
	};
}