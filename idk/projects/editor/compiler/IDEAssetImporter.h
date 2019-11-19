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
		ResourceBundle Get(PathHandle path) override;
		ResourceBundle Get(string_view mount_path) override;
		template<typename T> RscHandle<T> Get(string_view mount_path);

		/* FILE OPS */
		template<typename Res, typename = sfinae<has_tag_v<Res, Saveable>>> 
		void Save(RscHandle<Res> handle);

		template<typename Res, typename = sfinae<has_tag_v<Res, Saveable>>>
		void CopyTo(RscHandle<Res> handle, PathHandle path);

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