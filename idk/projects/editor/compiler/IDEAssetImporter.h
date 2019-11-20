#pragma once
#include <res/compiler/AssetImporter.h>

namespace idk
{
	class EditorAssetImporter
		: public AssetImporter
	{
	public:
		/* LOOPS */
		void CheckImportDirectory() override;

		/* EXPORT */
		void CleanBuildDirectory();
		void ExportCustomFiles();

		/* FILE GET */
		using AssetImporter::Get;
		ResourceBundle Get(PathHandle path) override;
		template<typename Res> vector<RscHandle<Res>> GetAll();
		opt<string> GetPath(const GenericResourceHandle& h);
		template<typename Res> opt<string> GetPath(RscHandle<Res>);

		/* FILE OPS */
		template<typename Res, typename = sfinae<has_tag_v<Res, Saveable>>> 
		void Save(RscHandle<Res> handle);

		template<typename Res, typename = sfinae<has_tag_v<Res, Saveable>>>
		void CopyTo(RscHandle<Res> handle, PathHandle path);

		template<typename Res, typename = sfinae<has_tag_v<Res, Saveable>>>
		void Rename(RscHandle<Res> handle, string_view mount_path);

		/* IMPORTING */
		template<typename Importer>
		void RegisterImporter(string_view ext);
	private:
		using MountPath = string;

		hash_table<MountPath, ResourceBundle> bundles;
		hash_table<GenericResourceHandle, string> pathback;
		hash_table<string_view, unique_ptr<class Importer>> importers;

		void ImportFile(PathHandle filepath);
		void LoadMeta(PathHandle meta_mount_path);
		void Init();
		void LateInit();
		void Shutdown();
	};
	

	template<typename Res>
	inline vector<RscHandle<Res>> EditorAssetImporter::GetAll()
	{
		vector<RscHandle<Res>> retval{};
		for (auto& [path, bundle] : bundles)
		{
			for (auto& elem : bundle.GetAll<Res>())
				retval.emplace_back(elem);
		}
		return retval;
	}

	template<typename Res>
	inline opt<string> EditorAssetImporter::GetPath(RscHandle<Res> h)
	{
		return GetPath(GenericResourceHandle{h});
	}

	template<typename Res, typename>
	void EditorAssetImporter::Save(RscHandle<Res> handle)
	{
		auto itr = pathback.find(GenericResourceHandle{ handle });
		if (itr != pathback.end())
		{
			auto outstream = PathHandle{ itr->second }.Open(FS_PERMISSIONS::WRITE);
			outstream << serialize_text(*handle);
		}
	}
	template<typename Res, typename>
	inline void EditorAssetImporter::CopyTo(RscHandle<Res> handle, PathHandle path)
	{
		auto itr = bundles.find(path.GetMountPath());
		if (itr == bundles.end())
		{
			auto outstream = path.Open(FS_PERMISSIONS::WRITE);
			outstream << serialize_text(*handle);
			// then let resource manager handle the reimport
		}
	}
}