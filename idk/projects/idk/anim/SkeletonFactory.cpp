#include "stdafx.h"
#include "SkeletonFactory.h"
#include <anim/Skeleton.h>

namespace idk::anim
{
	unique_ptr<Skeleton> SkeletonFactory::GenerateDefaultResource()
	{
		auto retval = std::make_unique<Skeleton>();
		Bone b;
		b._name = "ROOT";
		b._global_inverse_bind_pose = mat4{ };
		b._parent = -1;

		return retval;
	}
}