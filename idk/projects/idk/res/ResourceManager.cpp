#include "stdafx.h"

#include <sstream>
#include <ds/ranged_for.h>
#include <file/FileSystem.h>
#include <IncludeResources.h>
#include <serialize/serialize.h>
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

			static array<void(*)(ResourceManager*), sizeof...(Rs)> GenDefaults()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[](ResourceManager* resource_man)
					{
						if (auto loader = &resource_man->GetLoader<Rs>())
							resource_man->_default_resources[ResourceID<Rs>] = loader->GenerateDefaultResource();
					}...
				};
			}

			static array<void(*)(ResourceManager*), sizeof...(Rs)> InitFactories()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[](ResourceManager* resource_man)
					{
						if (auto loader = &resource_man->GetLoader<Rs>())
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
		_resource_tables   = detail::ResourceHelper::GenResourceTables();

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

		for (auto& elem : _resource_tables)
			elem.reset();

		for (auto& elem : _resource_factories)
		{
			elem.reset();
		}
		for (auto& elem : this->_extension_loaders)
		{
			elem.second.reset();
		}
	}

	void ResourceManager::WatchDirectory()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
			LoadFile(elem);

		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN))
			ReloadFile(elem);
	}
	
	FileResources ResourceManager::LoadFile(PathHandle file)
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
			if (meta_file)
			{
				std::stringstream s; 
				s << meta_file.Open(FS_PERMISSIONS::READ).rdbuf();

				auto metalist = parse_text<MetaFile>(s.str());
				return loader_itr->second->Create(file, metalist);
			}
			else
				return loader_itr->second->Create(file);
		}();

		_loaded_files.emplace_hint(find_file, file.GetMountPath(), resources);
		return resources;
	}

	FileResources ResourceManager::LoadFile(PathHandle file, const MetaFile& meta)
	{
		auto find_file = _loaded_files.find(string{ file.GetMountPath() });
		if (find_file != _loaded_files.end())
			return ReloadFile(file);

		if (!file)
			return FileResources{};

		auto loader_itr = _extension_loaders.find(string{ file.GetExtension() });
		if (loader_itr == _extension_loaders.end())
			return FileResources{};

		auto resources = [&]()
		{
			return loader_itr->second->Create(file, meta);
		}();

		_loaded_files.emplace_hint(find_file, file.GetMountPath(), resources);
		return resources;
	}

	FileResources ResourceManager::ReloadFile(PathHandle file)
	{
		auto find_file = _loaded_files.find(string{ file.GetMountPath() });
		if (find_file == _loaded_files.end())
			return FileResources();

		// save old meta
		auto ser = save_meta(find_file->second);

		// unload resources
		UnloadFile(file);

		// get loader
		auto loader_itr = _extension_loaders.find(string{ file.GetExtension() });
		if (loader_itr == _extension_loaders.end())
			return FileResources{};

		// reload resources
		auto stored = loader_itr->second->Create(file, ser);
		return _loaded_files.emplace_hint(find_file, string{ file.GetMountPath() }, stored)->second;
	}

	size_t ResourceManager::UnloadFile(PathHandle path_to_file)
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

	FileResources ResourceManager::GetFileResources(PathHandle path_to_file)
	{
		auto find_file = _loaded_files.find(string{ path_to_file.GetMountPath() });
		if (find_file != _loaded_files.end())
			return find_file->second;
		else
			return FileResources();
	}

	bool ResourceManager::Associate(string_view mount_path, GenericRscHandle f)
	{
		auto itr = _loaded_files.find(string{ mount_path });
		
		auto& filemeta = [&]() -> FileResources&
		{
			if (itr == _loaded_files.end())
			{
				auto [jtr, success] = _loaded_files.emplace(string{ mount_path }, FileResources{});
				assert(success);
				return jtr->second;
			}
			else
			{
				return itr->second;
			}
		}();
		auto find_f = std::find(filemeta.resources.begin(), filemeta.resources.end(), f);
		if (find_f == filemeta.resources.end())
		{
			filemeta.resources.emplace_back(f);
			f.visit([&](auto& elem) {

				auto& table = GetTable<typename std::decay_t<decltype(elem)>::Resource>();
				auto& control_block = table.find(elem.guid)->second;

				control_block.is_new = true;
			});
			return true;
		}
		else
			return false;
	}

	void ResourceManager::SaveDirtyMetadata()
	{
		auto& fs = Core::GetSystem<FileSystem>();
		for (auto& [filepath, resources] : _loaded_files)
		{
			bool dirty = false;
			for (auto& elem : resources.resources)
				elem.visit([&](auto& elem) {

				auto& table = GetTable<typename std::decay_t<decltype(elem)>::Resource>();
				auto& control_block = table.find(elem.guid)->second;

				dirty |= control_block.is_new;
				if constexpr (has_tag_v<decltype(elem), MetaTag>)
					dirty |= elem->_dirtymeta;
			});

			if (dirty)
			{
				auto saveus = save_meta(resources);
				std::stringstream stream;

				stream << serialize_text(saveus);
				auto meta_file = fs.Open(filepath + ".meta", FS_PERMISSIONS::WRITE, false);
				meta_file << stream.rdbuf();
				// mark as clean
				for (auto& elem : resources.resources)
					elem.visit([&](auto& elem) {
					auto& table = GetTable<typename std::decay_t<decltype(elem)>::Resource>();
					auto& control_block = table.find(elem.guid)->second;

					control_block.is_new = false;
					if constexpr (has_tag_v<decltype(elem), MetaTag>)
						elem->_dirtymeta = false;
				});
			}

		}
	}
}