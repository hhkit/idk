#include "stdafx.h"
#include "SceneFactory.h"
#include <res/MetaBundle.h>

namespace idk
{
	unique_ptr<Scene> SceneFactory::GenerateDefaultResource()
	{
		return std::make_unique<Scene>((unsigned char) 0x80);
	}

	unique_ptr<Scene> SceneFactory::Create() noexcept
	{
		// scenes are never created by factory
		assert(false);
		return unique_ptr<Scene>();
	}

	ResourceBundle SceneLoader::LoadFile(PathHandle) noexcept
	{
		// scenes are never imported
		assert(false); 
		return ResourceBundle();
	}
	ResourceBundle SceneLoader::LoadFile(PathHandle filepath, const MetaBundle& bundle)
	{
		return RscHandle<Scene>{bundle.metadatas[0].guid};
	}
}