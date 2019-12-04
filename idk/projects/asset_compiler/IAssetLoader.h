#pragma once
#include <idk.h>
#include <res/MetaBundle.h>
#include <res/CompiledAssets.h>

namespace idk
{
	struct AssetBundle
	{
		using AssetPair = std::pair<Guid, CompiledVariant>;
		MetaBundle metabundle; // the new bundle
		hash_table<Guid, CompiledVariant> assets; // the produced assets

		AssetBundle(const MetaBundle& bundle, std::initializer_list<AssetPair> assets);
		AssetBundle(const MetaBundle& bundle, span<AssetPair> assets);
	};

	class IAssetCompiler
	{
	public:
		 virtual opt<AssetBundle> LoadAsset(string_view full_path, const MetaBundle& bundle) = 0;
	};
}