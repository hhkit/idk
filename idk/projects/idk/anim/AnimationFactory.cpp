#include "stdafx.h"
#include "AnimationFactory.h"
#include <anim/Animation.h>

namespace idk::anim
{
	unique_ptr<Animation> AnimationFactory::GenerateDefaultResource()
	{
		return std::make_unique<Animation>();
	}
	unique_ptr<Animation> AnimationFactory::Create()
	{
		return std::make_unique<Animation>();
	}

	unique_ptr<Animation> AnimationFactory::Create(FileHandle filepath)
	{
		UNREFERENCED_PARAMETER(filepath);
		return unique_ptr<Animation>();
	}
}
