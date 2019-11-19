#include "pch.h"
#include "IDEAssetImporter.h"
#include <app/Application.h>
#include <res/MetaBundle.h>
#include <serialize/text.h>
#include <util/ioutils.h>
#include <gfx/ShaderGraph.h>
#include <editor/compiler/BypassImporter.h>
namespace idk
{
	namespace detail
	{
		template<typename T> struct AssetImporterHelper;

		template<typename ...Ts>
		struct AssetImporterHelper<std::tuple<Ts...>>
		{
			static constexpr auto GenSaveableImporterTable()
			{
				return array<void(*)(EditorAssetImporter*), ResourceCount>
				{
					[]([[maybe_unused]] EditorAssetImporter* editor)
					{
						if constexpr (has_extension_v<Ts>)
							editor->RegisterImporter<BypassImporter<Ts>>(Ts::ext);
					}
					...
				};
			}
		};

		using AIHelper = AssetImporterHelper<Resources>;
	}

	void EditorAssetImporter::CheckImportDirectory()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
			ImportFile(elem);

		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN))
			ImportFile(elem);
	}

	void EditorAssetImporter::CleanBuildDirectory()
	{
	}

	void EditorAssetImporter::ExportCustomFiles()
	{
	}

	ResourceBundle EditorAssetImporter::Get(string_view mount_path)
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
		{
			LoadMeta(filepath);
			return;
		}
			
		const auto itr = importers.find(filepath.GetExtension());
		if (itr != importers.end())
		{
			MetaBundle m;
			auto meta_path = PathHandle{ string{ filepath.GetMountPath() } +".meta" };
			{
				auto stream = meta_path.Open(FS_PERMISSIONS::READ);
				if (stream)
					parse_text(stringify(stream), m);
			}
			auto new_m = itr->second->Import(filepath, m);
			
			if (new_m != m)
			{
				auto stream = meta_path.Open(FS_PERMISSIONS::WRITE);
				stream << serialize_text(new_m);
			}
			return;
		}

		if (filepath.GetMountPath().find("/assets") != std::string_view::npos)
		{
			array<const char*, 2> args = { filepath.GetFullPath().data(), full_path.data() };
			Core::GetSystem<Application>().Exec("tools/compiler/idc.exe", span<const char*>(args));
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
			for (auto& elem : bundle.GetAll())
				pathback[elem] = meta_mount_path.GetMountPath();
			bundles[string{ meta_mount_path.GetMountPath() }] = std::move(bundle);
		}
		else
			bundles.erase(string{ meta_mount_path.GetMountPath() });
	}

	void EditorAssetImporter::Init()
	{
		constexpr auto importerjt = detail::AIHelper::GenSaveableImporterTable();
		for (auto& func : importerjt)
			func(this);

		RegisterImporter<BypassImporter<shadergraph::Graph>>(Material::ext);
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