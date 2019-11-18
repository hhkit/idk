#include "pch.h"
#include "IDEAssetImporter.h"
#include <app/Application.h>

namespace idk
{
	void EditorAssetImporter::CheckImportDirectory()
	{
		auto full_path = Core::GetSystem<FileSystem>().GetFullPath("/build");
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
		{
			if (elem.GetMountPath().find("/assets") != std::string_view::npos)
			{
				array<const char*, 2> args = { elem.GetFullPath().data(), full_path.data() };
				Core::GetSystem<Application>().Exec("tools/compiler/idc.exe", span<const char*>(args));
			}
		}
	}

	void EditorAssetImporter::Init()
	{
	}

	void EditorAssetImporter::Shutdown()
	{
	}
}