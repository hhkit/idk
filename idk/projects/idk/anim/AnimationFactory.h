#pragma once

#include <anim/Animation.h>
#include <res/ResourceFactory.h>
namespace idk::anim
{
	class AnimationFactory
		: public ResourceFactory<Animation>
	{
	public:
		unique_ptr<Animation> GenerateDefaultResource() override;
		unique_ptr<Animation> Create() override;
		//unique_ptr<Mesh> Create(const ResourceMeta&);
		unique_ptr<Animation> Create(FileHandle filepath) override;
	};
}
