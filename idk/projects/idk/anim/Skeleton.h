#pragma once
#include <idk.h>
#include <res/Resource.h>

namespace idk::anim
{
	struct Bone
	{
		string _name;
		int  _parent;
		mat4 _global_inverse_bind_pose;
		matrix_decomposition<real> _local_bind_pose;
	};

	class Skeleton
		: public Resource<Skeleton>
	{
	public:
		

		Skeleton() = default;
		Skeleton(const vector<Bone>& bones, const hash_table<string, size_t>& bone_table);

		const mat4& GetGlobalInverse() const { return _global_inverse; }
		const Bone* GetBone(string_view name) const;
		const vector<Bone>& data() const { return _bones; }

		void AddBone(string_view name, Bone b);
		void SetSkeletonTransform(const mat4& mat) { _global_inverse = mat; }

	private:
		mat4 _global_inverse;

		hash_table<string, size_t> _bone_table;
		vector<Bone> _bones;
	};
}