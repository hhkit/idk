#pragma once
#include <res/IAssetLoader.h>

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
}