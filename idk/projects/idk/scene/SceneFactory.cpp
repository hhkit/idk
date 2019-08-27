#include "stdafx.h"
#include "SceneFactory.h"

namespace idk
{
	unique_ptr<Scene> SceneFactory::Create()
	{
		return std::make_unique<Scene>();
	}

	unique_ptr<Scene> SceneFactory::Create(string_view)
	{
		return unique_ptr<Scene>();
	}
}