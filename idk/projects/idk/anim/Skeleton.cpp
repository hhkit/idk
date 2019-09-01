#include "stdafx.h"
#include "Skeleton.h"

namespace idk::anim
{
	Skeleton::Skeleton(const vector<Bone>& bones, const hash_table<string, size_t>& bone_table)
		:_bones{ bones }, _bone_table{ bone_table }
	{
	}
}
