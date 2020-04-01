#pragma once
#include "IAssetLoader.h"

namespace idk
{
	class FontAtlasCompiler
		: public IAssetCompiler
	{
	public:
		opt<AssetBundle> LoadAsset(string_view full_path, const MetaBundle& bundle) override;
	};
}