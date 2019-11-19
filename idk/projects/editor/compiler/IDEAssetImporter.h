#pragma once
#include <res/compiler/AssetImporter.h>

namespace idk
{
	class EditorAssetImporter
		: public AssetImporter
	{
	public:
		void CheckImportDirectory() override;
		ResourceBundle GetFile(string_view mount_path) override;

		template<typename Importer>
		void RegisterCustomImporter(string_view ext);
	private:
		using MountPath = string;

		hash_table<MountPath, ResourceBundle> bundles;
		hash_table<GenericResourceHandle, string> backpath;
		hash_table<string_view, unique_ptr<class Importer>> importers;

		void ImportFile(PathHandle filepath);
		void LoadMeta(PathHandle meta_mount_path);
		void Init();
		void LateInit();
		void Shutdown();
	};
}