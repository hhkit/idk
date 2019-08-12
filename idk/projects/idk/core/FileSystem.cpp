#include "stdafx.h"

#include "FileSystem.h"

#include <filesystem>
#include <utility>
#include <direct.h>
#include <tchar.h>
#include <stdio.h>  
#include <Shlwapi.h>

namespace FS = std::filesystem;


namespace idk {
	void FileSystem::Init()
	{
		Mount("C:\\Users\\Joseph\\Desktop\\GIT\\idk_legacy\\Koboru\\Koboru\\resource\\editor", "test");
	}
	void FileSystem::Run(float dt)
	{
		UNREFERENCED_PARAMETER(dt);

	}
	void FileSystem::Shutdown()
	{
	}
	FileSystem_ErrorCode FileSystem::Mount(const string& fullPath, const string& mountPath)
	{
		filesystem_internal::mount_t mount;
		mount.Init(fullPath, mountPath);
		return FILESYSTEM_OK;
	}
}