#pragma once

#include <anim/Skeleton.h>
#include <res/ResourceFactory.h>
namespace idk::anim
{
	class SkeletonFactory
		: public ResourceFactory<Skeleton>
	{
	public:	
		unique_ptr<Skeleton> GenerateDefaultResource() override;
		unique_ptr<Skeleton> Create() override;
		//unique_ptr<Mesh> Create(const ResourceMeta&);
		unique_ptr<Skeleton> Create(PathHandle filepath) override;

	};
}
