#pragma once
#include "IAssetLoader.h"
#include "DDSCompiler.h"

namespace idk
{
	class TextureCompiler
		: public DDSCompiler
	{
	public:
		opt<AssetBundle> LoadAsset(string_view full_path, const MetaBundle& bundle) override;
	};
}