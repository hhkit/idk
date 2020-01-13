#include <res/ResourceHandle.h>
#include "TextureCompiler.h"
#include <process.h>
#include <windows.h>
#include <filesystem>
namespace idk
{
	opt<AssetBundle> TextureCompiler::LoadAsset(string_view full_path, const MetaBundle& bundle)
	{
		namespace fs = std::filesystem;
		constexpr auto wrap = [](string_view str) -> string
		{
			return '\"' + string{ str} +'\"';
		};

		auto temp_dir = fs::temp_directory_path();
		auto out_path = temp_dir / fs::path{ full_path }.stem();
		auto tmp_path = out_path; tmp_path += ".tmp.dds";
		auto final_path = out_path; final_path += ".dds";

		auto retval = _spawnl(P_WAIT, "tools/nvtt/nvcompress.exe", 
			"nvcompress.exe",
			"-silent",
			"-bc3",
			wrap(full_path).data(),
			wrap(tmp_path.string()).data(),
			0);

		rename(tmp_path.string().data(), final_path.string().data());

		if (retval == 0)
		{
			auto retval = DDSCompiler::LoadAsset(final_path.string(), bundle);
			if (fs::exists(final_path))
				fs::remove(final_path);
			return retval;
		}

		return {};
	}
}
