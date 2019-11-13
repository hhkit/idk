#pragma once
#include <idk.h>
#include <res/MetaBundle.h>
#include <reflect/reflect.h>

namespace idk
{
	struct AssetBundle
	{
		MetaBundle metabundle; // the new bundle
		hash_table<Guid, reflect::dynamic> assets; // the produced assets
	};

	class IAssetCompiler
	{
	public:
		 virtual AssetBundle LoadAsset(string_view full_path, const MetaBundle& bundle) = 0;
	};
}