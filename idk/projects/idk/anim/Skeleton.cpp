#include "stdafx.h"
#include "Skeleton.h"
#include "math/matrix_transforms.h"
namespace idk::anim
{
	Skeleton::Skeleton(const vector<BoneData>& bones, const hash_table<string, size_t>& bone_table)
		:_bones{ bones }, _bone_table{ bone_table }
	{
	}

	const BoneData* Skeleton::GetBone(string_view name) const
	{
		auto res = _bone_table.find(name.data());
		if (res == _bone_table.end())
			return nullptr;

		return &_bones[res->second];
	}

	void Skeleton::AddBone(string_view name, BoneData b)
	{
		auto res = _bone_table.find(name.data());
		if (res == _bone_table.end())
		{
			_bone_table.emplace(string{ name }, _bones.size());
			_bones.emplace_back(b);
		}
	}
}
