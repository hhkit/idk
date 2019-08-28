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

	void ResourceManager::WatchDirectory()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
			LoadFile(elem);

		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN))
			ReloadFile(elem);
	}
	
	void ResourceManager::LoadDefaultResources()
	{
		//default_resources_ = detail::ResourceHelper::GenDefaults();
	}
	FileResources ResourceManager::LoadFile(FileHandle file)
	{
		auto find_file = _loaded_files.find(string{ file.GetMountPath() });
		if (find_file != _loaded_files.end())
			return find_file->second;

		auto path_to_meta = string{ file.GetMountPath() } + ".meta";

		auto& fs = Core::GetSystem<FileSystem>();
		fs.Exists(path_to_meta);
		auto meta_file = fs.GetFile(path_to_meta);

		if (!file)
			return FileResources{};

		auto loader_itr = _extension_loaders.find(string{ file.GetExtension() });
		if (loader_itr == _extension_loaders.end())
			return FileResources{};

		auto resources = [&]()
		{
			//if (meta_file)
			//	return loader_itr->second->Create(file, path_to_meta);
			//else
				return loader_itr->second->Create(file);
		}();

		_loaded_files.emplace_hint(find_file, file.GetMountPath(), resources);
		return resources;
	}
	FileResources ResourceManager::ReloadFile(FileHandle file)
	{
		auto find_file = _loaded_files.find(string{ file.GetMountPath() });
		if (find_file == _loaded_files.end())
			return FileResources();

		// save old meta
		auto ser = serialize(find_file->second);

		// unload resources
		UnloadFile(file);

		// get loader
		auto loader_itr = _extension_loaders.find(string{ file.GetExtension() });
		if (loader_itr == _extension_loaders.end())
			return FileResources{};

		// reload resources
		auto stored = loader_itr->second->Create(file, span<SerializedResourceMeta>{ser});
		return _loaded_files.emplace_hint(find_file, string{ file.GetMountPath() }, stored)->second;
	}
	size_t ResourceManager::UnloadFile(FileHandle path_to_file)
	{
		auto find_file = _loaded_files.find(string{ path_to_file.GetMountPath() });
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

	FileResources ResourceManager::GetFileResources(FileHandle path_to_file)
	{
		auto find_file = _loaded_files.find(string{ path_to_file.GetMountPath() });
		if (find_file != _loaded_files.end())
			return find_file->second;
		else
			return FileResources();
	}
}