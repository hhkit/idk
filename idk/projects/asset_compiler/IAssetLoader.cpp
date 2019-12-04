#include <idk.h>
#include <core/Core.h>
#include "IAssetLoader.h"

namespace idk
{
	AssetBundle::AssetBundle(const MetaBundle& bundle, std::initializer_list<AssetPair> _assets)
		: metabundle{bundle}
	{
		for (auto& [key, value] : _assets)
			assets.emplace(key, value);
	}

	AssetBundle::AssetBundle(const MetaBundle& bundle, span<AssetPair> assetspan)
		: metabundle{ bundle }
	{
		for (auto& [key, value] : assetspan)
			assets.emplace(key, value);
	}
}
