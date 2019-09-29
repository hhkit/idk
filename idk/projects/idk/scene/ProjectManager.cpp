#include "stdafx.h"
#include "ProjectManager.h"
#include <serialize/serialize.h>
#include <util/ioutils.h>
#include <scene/SceneManager.h>

namespace idk
{

	void ProjectManager::Init()
	{
	}

	void ProjectManager::LateInit()
	{
		// initialize project
		auto scene_stream = Core::GetSystem<FileSystem>().Open("/assets/scenes.idk", FS_PERMISSIONS::READ); 
		if (scene_stream)
			parse_text(stringify(scene_stream), Core::GetSystem<SceneManager>());
	}

	void ProjectManager::Shutdown()
	{

	}

	void ProjectManager::SaveProject()
	{
		Core::GetSystem<FileSystem>().Open("/assets/scenes.idk", FS_PERMISSIONS::WRITE) << serialize_text(Core::GetSystem<SceneManager>());
	}
}
