#include "stdafx.h"
#include "Skeleton.h"

namespace idk::anim
{
	Skeleton::Skeleton(const vector<Bone>& bones, const hash_table<string, size_t>& bone_table)
		:_bones{ bones }, _bone_table{ bone_table }
	{
	}

	void Skeleton::AddBone(string_view name, Bone b)
	{
		auto res = _bone_table.find(name.data());
		if (res == _bone_table.end())
		{
			_bone_table.emplace(string{ name }, _bones.size());
			_bones.emplace_back(b);
		}
	}
}
