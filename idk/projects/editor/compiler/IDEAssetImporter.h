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
	private:
		hash_table<string, ResourceBundle> bundles;

		void ImportFile(PathHandle filepath);
		void LoadMeta(PathHandle meta_mount_path);
		void Init();
		void LateInit();
		void Shutdown();
	};
}