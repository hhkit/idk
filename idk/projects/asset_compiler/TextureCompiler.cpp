#include <res/ResourceHandle.h>
#include "TextureCompiler.h"
#include <process.h>
#include <windows.h>

namespace idk
{
	AssetBundle TextureCompiler::LoadAsset(string_view full_path, const MetaBundle& bundle)
	{
		constexpr auto wrap = [](string_view str) -> string
		{
			return '\"' + string{ str} +'\"';
		};

		auto out_path = string{ full_path } +".dds";

		auto retval = _spawnl(P_WAIT, "tools/nvtt/nvcompress.exe", 
			"nvcompress.exe", 
			wrap(full_path).data(),
			wrap(out_path).data(),
			0);

		if (retval == 0)
			return DDSCompiler::LoadAsset(out_path, bundle);

		return AssetBundle{};
	}
}
