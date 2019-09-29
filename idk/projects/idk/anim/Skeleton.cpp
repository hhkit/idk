#include "stdafx.h"
#include "Skeleton.h"
#include "math/matrix_transforms.h"
namespace idk::anim
{
	Skeleton::Skeleton(const vector<Bone>& bones, const hash_table<string, size_t>& bone_table)
		:_bones{ bones }, _bone_table{ bone_table }
	{
	}

	const Skeleton::Bone* Skeleton::GetBone(string_view name) const
	{
		auto res = _bone_table.find(name.data());
		if (res == _bone_table.end())
			return nullptr;

		return &_bones[res->second];
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
	mat4 Skeleton::BonePose::compose() const
	{
		return translate(_translation) * mat4 { quat_cast<mat3>(_rotation) * scale(_scale) };
	}
}
