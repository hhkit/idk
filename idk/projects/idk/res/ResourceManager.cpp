#include "stdafx.h"

#include <file/FileSystem.h>
#include <IncludeResources.h>
#include "ResourceManager.h"


namespace idk
{
	ResourceManager* ResourceManager::instance = nullptr;

	namespace detail
	{
		template<typename T> struct ResourceManager_detail;

		template<typename ... Rs>
		struct ResourceManager_detail<std::tuple<Rs...>>
		{
			static array<shared_ptr<void>, sizeof...(Rs)> GenResourceTables()
			{
				return array<shared_ptr<void>, sizeof...(Rs)>{
					std::make_shared<ResourceManager::Storage<Rs>>()...
				};
			}

			static array<shared_ptr<void>, sizeof...(Rs)> GenDefaults()
			{
				return array<shared_ptr<void>, sizeof...(Rs)>{
					std::shared_ptr<Rs>()...
				};
			}
		};

		using ResourceHelper = ResourceManager_detail<Resources>;
	}

	void ResourceManager::Init()
	{
		instance = this;
		_resource_tables   = detail::ResourceHelper::GenResourceTables();
		LoadDefaultResources();

		auto& fs = Core::GetSystem<FileSystem>();
		auto exe_dir = std::string{ fs.GetExeDir() };
		fs.Mount(exe_dir + "/assets", "/assets");
		fs.SetAssetDir(exe_dir + "/assets");
	}

	void ResourceManager::Shutdown()
	{
		for (auto& elem : _resource_tables)
			elem.reset();

		for (auto& elem : _default_resources)
			elem.reset();
	}
	void ResourceManager::LoadDefaultResources()
	{
		//default_resources_ = detail::ResourceHelper::GenDefaults();
	}
	FileResources ResourceManager::LoadFile(string_view path_to_file)
	{
		auto find_file = _loaded_files.find(string{ path_to_file });
		if (find_file != _loaded_files.end())
			return find_file->second;

		auto path_to_meta = string{ path_to_file } + ".meta";

		auto& fs = Core::GetSystem<FileSystem>();
		auto file = fs.GetFile(path_to_file);
		auto meta_file = fs.GetFile(path_to_meta);

		if (!file)
			return FileResources{};

		auto loader_itr = _extension_loaders.find(string{ file.GetExtension() });

		if (loader_itr == _extension_loaders.end())
			return FileResources{};

		auto resources = [&]()
		{
			if (meta_file)
				return loader_itr->second->Create(path_to_file, path_to_meta);
			else
				return loader_itr->second->Create(path_to_file);
		}();

		_loaded_files.emplace_hint(find_file, path_to_file, resources);
		return resources;
	}
	FileResources ResourceManager::ReloadFile(std::string_view path_to_file)
	{
		auto find_file = _loaded_files.find(string{ path_to_file });
		if (find_file == _loaded_files.end())
			return FileResources();

		// save old meta
		auto saved_metas = find_file->second;

		// unload resources
		UnloadFile(path_to_file);
	}
	size_t ResourceManager::UnloadFile(std::string_view path_to_file)
	{
		auto find_file = _loaded_files.find(string{ path_to_file });
		if (find_file == _loaded_files.end())
			return 0;

		for (auto& elem : find_file->second.resources)
		{
			std::visit([this](auto handle) {
				Free(handle);
			}, elem._handle);
		}

		auto retval = find_file->second.resources.size();
		_loaded_files.erase(find_file);
		return retval;
	}

	FileResources ResourceManager::GetFileResources(string_view path_to_file)
	{
		auto find_file = _loaded_files.find(string{ path_to_file });
		if (find_file != _loaded_files.end())
			return find_file->second;
		else
			return FileResources();
	}
}