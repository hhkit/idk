#include <idk.h>
#include <res/Resource.h>
namespace idk::anim
{
	class Skeleton
		: public Resource<Skeleton>
	{
	public:
		struct Bone
		{
			string _name;
			int  _parent;
			mat4 _offset;
		};

		Skeleton() = default;
		Skeleton(const vector<Bone>& bones, const hash_table<string, size_t>& bone_table);

	private:
		
		mat4 _global_inverse;

		hash_table<string, size_t> _bone_table;
		vector<Bone> _bones;
	};
}