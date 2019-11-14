#pragma once
#include "IAssetLoader.h"

namespace idk
{
	class DDSCompiler
		: public IAssetCompiler
	{
	public:
		AssetBundle LoadAsset(string_view full_path, const MetaBundle& bundle) override;
	};
}