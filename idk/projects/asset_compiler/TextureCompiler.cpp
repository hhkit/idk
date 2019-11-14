#include <res/ResourceHandle.h>
#include "TextureCompiler.h"
#include <process.h>
#include <windows.h>

namespace idk
{
	AssetBundle TextureCompiler::LoadAsset(string_view full_path, const MetaBundle& bundle)
	{
		auto in_file = '\"' + string{ full_path } +'\"';
		auto out_file = static_cast<string>(Guid::Make()) +".dds";

		auto retval = _spawnl(P_WAIT, "tools/nvtt/nvcompress.exe", 
			"nvcompress.exe", 
			in_file.data(),
			out_file.data(),
			0);

		if (retval == 0)
			return DDSCompiler::LoadAsset(out_file, bundle);

		return AssetBundle{};
	}
}
