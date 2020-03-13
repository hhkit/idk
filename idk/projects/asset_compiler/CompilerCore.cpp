
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

#include <iostream>

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
		std::cout << "compiling";

		auto tmp = string{ fs::temp_directory_path().string() };
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
				{
					std::cout << " successfully opened meta";
					parse_text(stringify(str), bundle);
				}
				else
					std::cout << "failed to open meta";
			}

			// try to load the asset
			auto result = find_loader->second->LoadAsset(full_path, bundle);
			
			if (result)
			{
				// serialize the bundle
				if (bundle != result->metabundle)
				{
					auto tmp_meta_path = tmp + pathify.stem().string().data();
					{
						std::ofstream bundle_stream{ tmp_meta_path.sv() };
						bundle_stream << serialize_text(result->metabundle);
					}
					if (fs::exists(meta_path))
						fs::remove(meta_path);
					rename(tmp_meta_path.data(), meta_path.string().data());
				}

				auto last_write = std::chrono::system_clock::now().time_since_epoch().count();				
				auto compile_time_name = pathify.stem().string() + ".time";
				auto tmp_compile_path = tmp + compile_time_name.data();
				fs::create_directories(fs::path(time_dir.sv()).parent_path());
				{
					std::ofstream str{ tmp_compile_path, std::ios::binary };
					str << serialize_binary(last_write);
				}
				auto compile_time_path = time_dir + ".time";
				if (fs::exists(compile_time_path.sv()))
					fs::remove(compile_time_path.sv());
				rename(tmp_compile_path.data(), compile_time_path.data());


				// serialize the resources
				{
					for (auto& [guid, binary_resource] : result->assets)
					{
						bool res = std::visit([&](const auto& elem) -> bool
							{
								using T = std::decay_t<decltype(elem)>;
								auto stem = fs::path{ (string{ guid } +string{ T::ext }).sv() };
								auto temp = fs::absolute(fs::path{ tmp.sv() } / stem);
								auto dest = fs::absolute(fs::path{ destination.sv() } / stem);

								if constexpr (has_extension_v<T>)
								{
									static_assert(T::ext[0] == '.', "Extension must begin with a .");
                                    if constexpr (has_tag_v<T, Saveable>)
                                    {
										std::ofstream resource_stream{ temp };
										resource_stream << serialize_text(elem);
                                    }
                                    else
                                    {
										std::ofstream resource_stream{ temp, std::ios::binary };
										resource_stream << serialize_binary(elem);
                                    }

									if(fs::exists(dest))
										fs::remove(dest);

									rename(temp.string().data(), dest.string().data());
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
		else
			std::cout << "could not find loader for extension " << pathify.extension().generic_string();
	}
}