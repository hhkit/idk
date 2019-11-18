#include <idk.h>
#include <core/Core.h>
#include "CompilerCore.h"

#include <filesystem>
#include <serialize/binary.h>
#include <util/ioutils.h>
#include <idk_config.h>
#include <res/ResourceHandle.h>
#include <res/ResourceExt.h>

#include "IAssetLoader.h"

namespace fs = std::filesystem;

template<typename T, typename U>
struct has_value {};

namespace idk
{
	CompilerCore::CompilerCore()
	{
	}
	void CompilerCore::Compile(string_view full_path)
	{
		auto pathify = fs::path{ full_path };
		auto find_loader = _loaders.find(pathify.extension().generic_string());

		if (find_loader != _loaders.end())
		{
			// try to find a matching .meta
			auto meta_path = [&]()
			{
				auto retval = pathify;
				return retval += ".meta";
			}();

			MetaBundle bundle; // try to get a bundle
			{
				std::ifstream str{ meta_path };
				if (str)
					parse_text(stringify(str), bundle);
			}

			// try to load the asset
			auto result = find_loader->second->LoadAsset(full_path, bundle);
			
			if (result)
			{
				// serialize the bundle
				{
					std::ofstream bundle_stream{ meta_path };
					bundle_stream << serialize_text(result->metabundle);
				}

				// serialize the resources
				{
					for (auto& [guid, binary_resource] : result->assets)
					{
						// if you're here, you're trying to get compiled so only binary here
						std::visit([&](const auto& elem) -> bool
							{
								using T = std::decay_t<decltype(elem)>;
								if constexpr (has_extension_v<T>)
								{
									static_assert(T::ext[0] == '.', "Extension must begin with a .");
									std::ofstream resource_stream{ string{guid} +string{T::ext} };
									resource_stream << serialize_binary(elem);
									return true;
								}
								else
								{
									static_assert(false, "At least one type does not have its EXTENSION() registered!");
									return false;
								}
							}, binary_resource);
					}
				}
			}
		}
	}
}