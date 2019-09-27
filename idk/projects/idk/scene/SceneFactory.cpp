#include "stdafx.h"
#include "SceneFactory.h"

namespace idk
{
	unique_ptr<Scene> SceneFactory::GenerateDefaultResource()
	{
		return std::make_unique<Scene>((unsigned char) 0x80);
	}

	unique_ptr<Scene> SceneFactory::Create()
	{
		assert(false);
		return unique_ptr<Scene>();
	}

	ResourceBundle SceneLoader::LoadFile(PathHandle filepath)
	{
		return ResourceBundle();
	}
}