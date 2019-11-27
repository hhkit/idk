#include "stdafx.h"
#include "SkeletonFactory.h"
#include <anim/Skeleton.h>

namespace idk::anim
{
	unique_ptr<Skeleton> SkeletonFactory::GenerateDefaultResource()
	{
		auto retval = std::make_unique<Skeleton>();
		BoneData b;
		b.name = "ROOT";
		b.global_inverse_bind_pose = mat4{ };
		b.parent = -1;

		return retval;
	}
}