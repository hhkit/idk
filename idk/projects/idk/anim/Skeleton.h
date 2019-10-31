#pragma once
#include <idk.h>
#include <res/Resource.h>
#include <math/matrix_decomposition.h>

namespace idk::anim
{
	struct BoneData
	{
		string _name;
		int  _parent = 0;
		mat4 _global_inverse_bind_pose;
		quat _bone_pivot;
		matrix_decomposition<real> _local_bind_pose;
	};

	class Skeleton
		: public Resource<Skeleton>
	{
	public:
		

		Skeleton() = default;
		Skeleton(const vector<BoneData>& bones, const hash_table<string, size_t>& bone_table);

		const mat4& GetGlobalInverse() const { return _global_inverse; }
		vector<std::pair<string_view, mat4>> GetBindPose() const;
		const BoneData* GetBone(string_view name) const;
		const vector<BoneData>& data() const { return _bones; }

		void AddBone(string_view name, BoneData b);
		void SetSkeletonTransform(const mat4& mat) { _global_inverse = mat; }
		
	private:
		mat4 _global_inverse;

		hash_table<string, size_t> _bone_table;
		vector<BoneData> _bones;
	};
}