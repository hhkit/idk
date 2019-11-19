#pragma once
#include <idk.h>
#include <res/MetaBundle.h>
#include <res/compiler/CompiledAssets.h>

namespace idk
{
	struct AssetBundle
	{
		using AssetPair = std::pair<const Guid, CompiledVariant>;
		MetaBundle metabundle; // the new bundle
		hash_table<Guid, CompiledVariant> assets; // the produced assets

		AssetBundle(const MetaBundle& bundle, std::initializer_list<AssetPair> assets);
	};

	class IAssetCompiler
	{
	public:
		 virtual opt<AssetBundle> LoadAsset(string_view full_path, const MetaBundle& bundle) = 0;
	};
}