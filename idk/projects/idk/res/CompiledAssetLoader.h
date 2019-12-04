#pragma once
#include <res/IAssetLoader.h>
#include <res/ResourceManager.h>
#include <serialize/binary.h>
#include <serialize/text.h>
#include <util/ioutils.h>
namespace idk
{
	template<typename CompiledAsset, typename EngineResource, bool Binary = true>
	class CompiledAssetLoader
		: public IAssetLoader
	{
	public:
		static constexpr auto ext = CompiledAsset::ext;
		void LoadAsset(PathHandle handle) override;
	};

	template<typename CompiledAsset, typename EngineResource, bool Binary>
	inline void CompiledAssetLoader<CompiledAsset, EngineResource, Binary>::LoadAsset(PathHandle handle)
	{
		auto res = [&]()
		{
			if constexpr (Binary)
			{
				auto istream = handle.Open(FS_PERMISSIONS::READ, true);
				return parse_binary<CompiledAsset>(stringify(istream));
			}
			else
			{
				auto istream = handle.Open(FS_PERMISSIONS::READ, false);
				return parse_text<CompiledAsset>(stringify(istream));
			}
		}();
		if (res)
		{
			Guid guid{ handle.GetStem() };
			Core::GetResourceManager().LoaderEmplaceResource<EngineResource>(guid, std::move(*res));
		}
	}
}