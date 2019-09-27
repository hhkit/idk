#pragma once

#include <anim/Skeleton.h>
#include <res/EasyFactory.h>
namespace idk::anim
{
	class SkeletonFactory
		: public EasyFactory<Skeleton>
	{
	public:	
		unique_ptr<Skeleton> GenerateDefaultResource() override;
	};
}
