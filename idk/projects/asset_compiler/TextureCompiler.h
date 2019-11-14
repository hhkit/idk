#pragma once
#include "IAssetLoader.h"
#include "DDSCompiler.h"

namespace idk
{
	class TextureCompiler
		: public DDSCompiler
	{
	public:
		AssetBundle LoadAsset(string_view full_path, const MetaBundle& bundle) override;
	};
}