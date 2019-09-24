#include "stdafx.h"

#include <sstream>
#include <ds/ranged_for.h>
#include <file/FileSystem.h>
#include <IncludeResources.h>
#include <serialize/serialize.h>
#include "ResourceManager.h"


namespace idk
{
	namespace detail
	{
		template<typename T> struct ResourceManager_detail;

		template<typename ... Rs>
		struct ResourceManager_detail<std::tuple<Rs...>>
		{
			static array<shared_ptr<void>, sizeof...(Rs)> GenResourceTables()
			{
				return array<shared_ptr<void>, sizeof...(Rs)>{
					std::make_shared<ResourceManager::ResourceStorage<Rs>>()...
				};
			}

			static array<void(*)(ResourceManager*), sizeof...(Rs)> GenDefaults()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[](ResourceManager* resource_man)
					{
						if (auto loader = &resource_man->GetFactory<Rs>())
							resource_man->_default_resources[ResourceID<Rs>] = loader->GenerateDefaultResource();
					}...
				};
			}

			static array<void(*)(ResourceManager*), sizeof...(Rs)> InitFactories()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[](ResourceManager* resource_man)
					{
						if (auto loader = &resource_man->GetFactory<Rs>())
							loader->Init();
					}...
				};
			}
		};

		using ResourceHelper = ResourceManager_detail<Resources>;
	}

	void ResourceManager::Init()
	{
		instance = this;
		_resource_table = detail::ResourceHelper::GenResourceTables();

		auto& fs = Core::GetSystem<FileSystem>();
		auto exe_dir = string{ fs.GetExeDir() };
		fs.Mount(exe_dir + "/assets", "/assets");
		fs.SetAssetDir(exe_dir + "/assets");

        fs.Mount(exe_dir + "/engine_data", "/engine_data");
	}

	void ResourceManager::LateInit()
	{
		for (auto& func : detail::ResourceHelper::InitFactories())
			func(this);
		for (auto& func : detail::ResourceHelper::GenDefaults())
			func(this);
	}

	void ResourceManager::Shutdown()
	{
		for (auto& elem : reverse(_default_resources))
			elem.reset();

		for (auto& elem : _resource_table)
			elem.reset();

		for (auto& elem : _factories)
			elem.reset();

		for (auto& elem : _file_loader)
			elem.second.reset();
	}

	IFileLoader* ResourceManager::GetLoader(string_view extension)
	{
		auto itr = _file_loader.find(string{ extension });
		if (itr == _file_loader.end())
			return nullptr;

		return itr->second.get();
	}

	void ResourceManager::WatchDirectory()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
			Load(elem);
		
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN))
			Load(elem);
	}

	ResourceManager::GeneralLoadResult ResourceManager::Load(PathHandle path)
	{
		auto* loader = GetLoader(path.GetExtension());
		if (loader == nullptr)
			return ResourceLoadError::ExtensionNotRegistered;

		if (!path)
			return ResourceLoadError::FileDoesNotExist;

		auto emplace_path = string{ path.GetMountPath() };

		// TODO: Meta handling

		// unload the file if loaded
		{
			auto itr = _loaded_files.find(emplace_path);
			if (itr != _loaded_files.end())
			{
				// release all resources attached to file
				for (auto& elem : itr->second.GetAll())
					std::visit([&](const auto& handle) { Release(handle); }, elem);
				_loaded_files.erase(itr);
			}
		}

		// reload the file
		auto bundle = loader->LoadFile(path);
		auto [itr, success] = _loaded_files.emplace(emplace_path, bundle);
		assert(success);

		// set path
		for (auto& elem : bundle.GetAll())
			std::visit(
				[&](const auto& handle) 
				{
					using Res = std::decay_t<decltype(handle)>::Resource;

					auto* cb = GetControlBlock(handle);
					assert(cb);
					cb->path = emplace_path;
				}
			, elem);

		return bundle;
	}
	
}