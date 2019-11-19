#include "pch.h"
#include "IDEAssetImporter.h"
#include <app/Application.h>
#include <res/MetaBundle.h>
#include <serialize/text.h>
#include <util/ioutils.h>

namespace idk
{
	void EditorAssetImporter::CheckImportDirectory()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
			ImportFile(elem);

		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN))
			ImportFile(elem);
	}

	ResourceBundle EditorAssetImporter::GetFile(string_view mount_path)
	{
		auto itr = bundles.find(string{ mount_path });
		if (itr != bundles.end())
			return itr->second;
		return ResourceBundle();
	}

	void EditorAssetImporter::ImportFile(PathHandle filepath)
	{
		const auto full_path = Core::GetSystem<FileSystem>().GetFullPath("/build");
		if (filepath.GetExtension() == ".meta")
			LoadMeta(filepath);
		else
		{
			if (filepath.GetMountPath().find("/assets") != std::string_view::npos)
			{
				array<const char*, 2> args = { filepath.GetFullPath().data(), full_path.data() };
				Core::GetSystem<Application>().Exec("tools/compiler/idc.exe", span<const char*>(args));
			}
		}
	}

	void EditorAssetImporter::LoadMeta(PathHandle meta_mount_path)
	{
		auto stream = meta_mount_path.Open(FS_PERMISSIONS::READ);
		auto mb = parse_text<MetaBundle>(stringify(stream));
		if (mb)
		{
			ResourceBundle bundle;
			for (auto& elem : mb->metadatas)
			{
				GenericResourceHandle insertme{ elem.guid, elem.t_hash };
				std::visit([&bundle](const auto& guid){ bundle.Add(guid); }, insertme);
			}
			bundles[string{ meta_mount_path.GetMountPath() }] = std::move(bundle);
		}
		else
			bundles.erase(string{ meta_mount_path.GetMountPath() });
	}

	void EditorAssetImporter::Init()
	{
	}

	void EditorAssetImporter::LateInit()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/build", FS_FILTERS::ALL))
			ImportFile(elem);
	}

	void EditorAssetImporter::Shutdown()
	{
	}
}