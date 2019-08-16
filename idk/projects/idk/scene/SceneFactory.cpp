#include "stdafx.h"
#include "SceneFactory.h"

namespace idk
{
	unique_ptr<Scene> SceneFactory::Create()
	{
		return std::make_unique<Scene>();
	}

	unique_ptr<Scene> SceneFactory::Create(const ResourceMeta&)
	{
		return unique_ptr<Scene>();
	}
}