#include "stdafx.h"
#include "ProjectManager.h"
#include <serialize/serialize.h>
#include <util/ioutils.h>

namespace idk
{

	void ProjectManager::Init()
	{
	}

	void ProjectManager::LateInit()
	{
		Core::GetSystem<FileSystem>().Open(project_path, FS_PERMISSIONS::READ);
		// initialize project
	}

	void ProjectManager::Shutdown()
	{

	}
}
