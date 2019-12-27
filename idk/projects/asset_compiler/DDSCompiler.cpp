#include <fstream>

#include <idk.h>
#include <idk_config.h>
#include <res/ResourceHandle.inl>
#include <res/MetaBundle.inl>
#include <gfx/Texture.h>
#include <gfx/CompiledTexture.h>
#include <serialize/text.inl>
#include <util/ioutils.h>
#include <reflect/reflect.inl>
#include <reflect/dynamic.inl>

#include "DDSCompiler.h"

namespace idk
{
	opt<AssetBundle> DDSCompiler::LoadAsset(string_view full_path, const MetaBundle& bundle)
	{
		auto binary_data = [&]() -> string
		{
			std::ifstream stream;
			stream.open(full_path.data(), std::ios::binary);
			IDK_ASSERT(stream);
			return stringify(stream);
		}();

		auto [t_guid, t_meta] = [&]()
		{
			auto res = bundle.FetchMeta<Texture>();
			return res ? std::make_pair(res->guid, *res->GetMeta<Texture>()) : std::make_pair(Guid::Make(), Texture::Metadata{});
		}();

		auto updated_meta_bundle = bundle;
		updated_meta_bundle.metadatas = { SerializedMeta{ t_guid, "", string{reflect::get_type<Texture>().name()}, serialize_text(t_meta) } };

		
		// compile the texture

		CompiledTexture t;
		t.filter_mode = t_meta.filter_mode;
		t.internal_format = t_meta.internal_format;
		t.is_srgb = t_meta.is_srgb;
		t.filter_mode = t_meta.filter_mode;
		t.pixel_buffer.insert(t.pixel_buffer.end(), binary_data.begin(), binary_data.end());

		return AssetBundle{ updated_meta_bundle, {{ t_guid, std::move(t)} } };
	}
}
