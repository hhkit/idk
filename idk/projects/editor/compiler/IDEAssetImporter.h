#pragma once
#include <res/compiler/AssetImporter.h>

namespace idk
{
	class EditorAssetImporter
		: public AssetImporter
	{
	public:
		void CheckImportDirectory() override;
	private:
		void Init();
		void Shutdown();
	};
}