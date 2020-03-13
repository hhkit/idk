#include <res/ResourceHandle.h>
#include "TextureCompiler.h"
#include <process.h>
#include <windows.h>
#include <filesystem>
#include <iostream>

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
		
		auto stringed_meta = bundle.FetchMeta<Texture>();
		auto meta = stringed_meta->GetMeta<Texture>();

		TextureMeta texture_meta = meta ? *meta : TextureMeta{};

		if (texture_meta.is_srgb)
			std::cout << "exporting gammacompressed texture" << "\n";
		else
			std::cout << "exporting linear texture" << "\n";
		
		auto retval =
			[&]()
		{
			if (texture_meta.is_srgb)
			{
				return _spawnl(P_WAIT, "tools/nvtt/nvcompress.exe",
					"nvcompress.exe",
					"-silent",
					"-bc3",
					wrap(full_path).data(),
					wrap(tmp_path.string()).data(),
					0);
			}
			else
			{
				return _spawnl(P_WAIT, "tools/nvtt/nvcompress.exe",
					"nvcompress.exe",
					"-silent",
					"-bc3",
					wrap(full_path).data(),
					wrap(tmp_path.string()).data(),
					0);
			}
		}();

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
