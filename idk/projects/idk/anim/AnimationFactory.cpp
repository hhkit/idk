#include "stdafx.h"
#include "AnimationFactory.h"
#include <anim/Animation.h>

namespace idk::anim
{
	unique_ptr<Animation> AnimationFactory::Create()
	{
		return unique_ptr<Animation>();
	}

	unique_ptr<Animation> AnimationFactory::Create(FileHandle filepath)
	{
		return unique_ptr<Animation>();
	}
}
