#include "stdafx.h"
#include "SkeletonFactory.h"
#include <anim/Skeleton.h>

namespace idk::anim
{
	unique_ptr<Skeleton> SkeletonFactory::Create()
	{
		auto retval = std::make_unique<Skeleton>();
		Skeleton::Bone b;
		b._name = "ROOT";
		b._offset = mat4{ };
		b._parent = -1;

		return retval;
	}
	unique_ptr<Skeleton> SkeletonFactory::Create(FileHandle filepath)
	{
		UNREFERENCED_PARAMETER(filepath);
		auto retval = std::make_unique<Skeleton>();
		Skeleton::Bone b;
		b._name = "ROOT";
		b._offset = mat4{ };
		b._parent = -1;

		return retval;
	}
}