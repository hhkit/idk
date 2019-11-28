#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <res/ResourceExt.h>
#include <math/matrix_decomposition.h>

namespace idk::anim
{
	struct BoneData
	{
		string name;
		int  parent = 0;
		vector<int> children;
		mat4 global_inverse_bind_pose;
		quat pre_rotation, post_rotation;
		matrix_decomposition<real> local_bind_pose;
	};

	class Skeleton
		: public Resource<Skeleton>
	{
	public:
		mat4 _global_inverse;
		hash_table<string, size_t> _bone_table;
		vector<BoneData> _bones;

		Skeleton() = default;
		Skeleton(const vector<BoneData>& bones, const hash_table<string, size_t>& bone_table);

		const mat4& GetGlobalInverse() const { return _global_inverse; }
		vector<std::pair<string_view, mat4>> GetBindPose() const;
		const BoneData* GetBone(string_view name) const;
		const vector<BoneData>& data() const { return _bones; }

		void AddBone(string_view name, BoneData b);
		void SetSkeletonTransform(const mat4& mat) { _global_inverse = mat; }

		EXTENSION(".idsktn");
	};
}