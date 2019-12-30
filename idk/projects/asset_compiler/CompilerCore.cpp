
#include <filesystem>
#include <fstream>

#include <idk.h>
#include <idk_config.h>
#include <core/Core.h>

#include <serialize/text.inl>
#include <serialize/binary.inl>
#include <reflect/reflect.inl>
#include <util/ioutils.h>
#include <res/ResourceHandle.inl>
#include <res/ResourceExtension.h>

#include "CompilerCore.h"
#include "IAssetLoader.h"

namespace fs = std::filesystem;

template<typename T, typename U>
struct has_value {};

namespace idk
{
	CompilerCore::CompilerCore()
	{
		char buffer[MAX_PATH] = { 0 };

		// Get the program directory
		int bytes = GetModuleFileNameA(NULL, buffer, MAX_PATH);
		auto exe_dir = string{ buffer };
		exe_dir = exe_dir.substr(0, exe_dir.find_last_of("\\"));

		fs::current_path(exe_dir.data());
	}
	void CompilerCore::SetDestination(string_view dest)
	{
		destination = string{ dest };
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
						bool res = std::visit([&](const auto& elem) -> bool
							{
								using T = std::decay_t<decltype(elem)>;
								if constexpr (has_extension_v<T>)
								{
									static_assert(T::ext[0] == '.', "Extension must begin with a .");
                                    if constexpr (has_tag_v<T, Saveable>)
                                    {
                                        std::ofstream resource_stream{ destination + "/" + string{guid} +string{T::ext} };
                                        resource_stream << serialize_text(elem);
                                    }
                                    else
                                    {
                                        std::ofstream resource_stream{ destination + "/" + string{guid} +string{T::ext}, std::ios::binary };
                                        resource_stream << serialize_binary(elem);
                                    }
									return true;
								}
								else
								{
									static_assert(false, "At least one type does not have its EXTENSION() registered!");
									return false;
								}
							}, binary_resource);

                        // testing parse
                        //if (res)
                        //{
                        //    std::visit([&](const auto& elem)
                        //    {
                        //        using T = std::decay_t<decltype(elem)>;
                        //        if constexpr (has_extension_v<T> && !has_tag_v<T, Saveable>)
                        //        {
                        //            std::ifstream in{ destination + "/" + string{guid} +string{T::ext}, std::ios::binary};
                        //            auto out = parse_binary<T>(binarify(in));
                        //        }
                        //    }, binary_resource);
                        //}
					}
				}
			}
		}
	}
}