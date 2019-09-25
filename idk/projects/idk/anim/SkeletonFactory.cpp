#include "stdafx.h"
#include "SkeletonFactory.h"
#include <anim/Skeleton.h>

namespace idk::anim
{
	unique_ptr<Skeleton> SkeletonFactory::GenerateDefaultResource()
	{
		auto retval = std::make_unique<Skeleton>();
		Skeleton::Bone b;
		b._name = "ROOT";
		b._offset = mat4{ };
		b._parent = -1;

		return retval;
	}
}