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

	private:
		
		mat4 _global_inverse;
		vector<Bone> _bones;
	};
}