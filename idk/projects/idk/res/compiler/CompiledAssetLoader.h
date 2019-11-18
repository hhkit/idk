#pragma once
#include <idk.h>
#include <res/FileLoader.h>
#include <serialize/binary.h>
#include <util/ioutils.h>
namespace idk
{
	template<typename EngineResource, typename CompiledAsset>
	class CompiledAssetLoader
		: public ResourceFactory<EngineResource>
	{
	public:
		unique_ptr<Resource> GenerateDefaultResource() override;

		unique_ptr<Resource> Create() override
		{
			if constexpr (std::is_default_constructible_v<EngineResource>)
				return std::make_unique<EngineResource>();
			else
				return unique_ptr<EngineResource>();
		}

		unique_ptr<Resource> Create(PathHandle path_to_single_file) override
		{
			static_assert(std::is_constructible_v<EngineResource, CompiledAsset>, "Engine Resource must be constructible from Compiled Asset!");
			auto stream = path_to_single_file.Open();
			auto parsed = parse_text<CompiledAsset>(stringify(stream));

			if (parsed)
				return std::make_unique<EngineResource>(*parsed);
			else
				return unique_ptr<EngineResource>();
		}
	};

	template<typename EngineResource, typename CompiledAsset>
	unique_ptr<typename CompiledAssetLoader<EngineResource, CompiledAsset>::Resource> CompiledAssetLoader<EngineResource, CompiledAsset>::GenerateDefaultResource()
	{
		if constexpr (std::is_default_constructible_v<EngineResource>)
			return std::make_unique<EngineResource>();
		else
			return unique_ptr<EngineResource>();
	}
}