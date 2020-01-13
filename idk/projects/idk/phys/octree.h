#pragma once
#include <idk.h>
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
	using octree_obj = Handle<Collider>;

	struct octree_node_data
	{
		octree_obj object{};
		Octant octant{ NONE };
		unsigned depth{0};

		aabb object_bounds{};
		// unique_ptr<octree_node_data> next{ nullptr };
		
	};
	struct octree_node
	{
		static constexpr size_t SPLIT_THRESHOLD = 1;
		aabb bounds;

		// on demand splitting/merging
		void split();
		void split_and_reinsert();
		void unsplit();

		// objects
		hash_table<octree_obj, octree_node_data> object_list;

		// split
		shared_ptr<octree_node> children[8]{ nullptr };
	};

	struct octree
	{
	public:
		octree() = default;

		// Preallocate the tree
		octree(vec3 center, float width, unsigned depth);

		void insert(octree_obj object);
	private:
		shared_ptr<octree_node> _root{};

		void insert_data(shared_ptr<octree_node> node, const octree_obj& key, octree_node_data& data);
		//unsigned _max_depth = 5;
	};
}