#include "stdafx.h"

#include <sstream>

#include <ds/ranged_for.h>
#include <file/FileSystem.h>
#include <res/MetaBundle.h>
#include <serialize/serialize.h>
#include <util/ioutils.h>
#include <res/SaveableResourceLoader.h>

#include <IncludeResources.h>

#include "ResourceManager.h"


namespace idk
{
	namespace detail
	{
		template<typename T> struct ResourceManager_detail;

		template<typename ... Rs>
		struct ResourceManager_detail<std::tuple<Rs...>>
		{
			constexpr static array<shared_ptr<void>, sizeof...(Rs)> GenResourceTables()
			{
				return array<shared_ptr<void>, sizeof...(Rs)>{
					std::make_shared<ResourceManager::ResourceStorage<Rs>>()...
				};
			}

			constexpr static array<void(*)(ResourceManager*), sizeof...(Rs)> ReleaseTableResources()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[](ResourceManager* resource_man)
					{
						if (auto table = &resource_man->GetTable<Rs>())
							table->clear();
					}...
				};
			}

			constexpr static array<void(*)(ResourceManager*), sizeof...(Rs)> GenDefaults()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[](ResourceManager* resource_man)
					{
						if (auto loader = &resource_man->GetFactoryRes<Rs>())
							resource_man->_default_resources[ResourceID<Rs>] = loader->GenerateDefaultResource();
					}...
				};
			}

			constexpr static array<void(*)(ResourceManager*), sizeof...(Rs)> CreateSaveableLoaders()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[]([[maybe_unused]] ResourceManager* resource_man)
					{
						if constexpr (has_tag_v<Rs, Saveable>)
							if constexpr (std::is_default_constructible_v<Rs>)
								if (resource_man->GetLoader(Rs::ext) == nullptr)
									resource_man->RegisterLoader<SaveableResourceLoader<Rs>>(Rs::ext);
					}...
				};
			}

			constexpr static array<void(*)(ResourceManager*), sizeof...(Rs)> CreateNewResourceVectors()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[]([[maybe_unused]] ResourceManager* resource_man)
					{
						resource_man->_new_resources[BaseResourceID<Rs>] = std::make_shared<vector<RscHandle<Rs>>>();
					}...
				};
			}

			constexpr static array<void(*)(ResourceManager*), sizeof...(Rs)> DumpNewResourceVectors()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[]([[maybe_unused]] ResourceManager* resource_man)
					{
						resource_man->GetNewVector<Rs>().clear();
					}...
				};
			}

			constexpr static array<void(*)(ResourceManager*), sizeof...(Rs)> InitFactories()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[](ResourceManager* resource_man)
					{
						if (auto loader = &resource_man->GetFactoryRes<Rs>())
							loader->Init();
					}...
				};
			}

			constexpr static array<void(*)(ResourceManager*), sizeof...(Rs)> SaveFiles()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
					[]([[maybe_unused]] ResourceManager* resource_man)
					{
						if constexpr (has_tag_v<Rs, Saveable>)
						{
							if constexpr (Rs::autosave)
								for (const auto& [guid, res_cb] : resource_man->GetTable<Rs>())
								{
									const auto h = RscHandle<Rs>{ guid };
									if (h->IsDirty())
									{
										resource_man->Save(h);
										h->Clean();
									}
								}
						}
					}...
				};
			}
		};

		using ResourceHelper = ResourceManager_detail<Resources>;
	}

	void ResourceManager::Init()
	{
		constexpr static auto saveable_table = detail::ResourceHelper::CreateSaveableLoaders();
		for (auto& func : saveable_table)
			func(this);

		constexpr static auto new_vector_table = detail::ResourceHelper::CreateNewResourceVectors();
		for (auto& func : new_vector_table)
			func(this);

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
		constexpr static auto init_table = detail::ResourceHelper::InitFactories();
		constexpr static auto defaults_table = detail::ResourceHelper::GenDefaults();
		for (auto& func : init_table)
			func(this);
		for (auto& func : defaults_table)
			func(this);

		for (auto& elem : Core::GetSystem<FileSystem>().GetEntries("/assets", FS_FILTERS::ALL))
			Load(elem);
	}

	void ResourceManager::Shutdown()
	{
		constexpr static auto release_tables = detail::ResourceHelper::ReleaseTableResources();

		for (auto& elem : reverse(_default_resources))
			elem.reset();

		for (auto& elem : release_tables)
			elem(this);

		for (auto& elem : reverse(_resource_table))
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

	void ResourceManager::EmptyNewResources()
	{
		static constexpr auto dump_new_table = detail::ResourceHelper::DumpNewResourceVectors();

		for (auto& fn : dump_new_table)
			fn(this);
	}

	void ResourceManager::SaveDirtyFiles()
	{
		static constexpr auto save_files = detail::ResourceHelper::SaveFiles();

		for (auto& fn : save_files)
			fn(this);
	}

	void ResourceManager::SaveDirtyMetadata()
	{
		for (auto& [path, resource] : _loaded_files)
		{
			auto dirty = resource.is_new; // is the resource brand new?

			if (!dirty) // are any resource metas dirty?
				for (auto& elem : resource.bundle.GetAll())
				{ 
					dirty = std::visit([&](const auto& handle) -> bool
						{
							using Res = typename std::decay_t<decltype(handle)>::Resource;
							if constexpr (has_tag_v<Res, MetaTag>)
								return handle->_dirtymeta;
							else
								return false;
						}, elem);
				}

			if (dirty)
			{
				// save the .meta file
				MetaBundle m;

				for (auto& elem : resource.bundle.GetAll())
				{
					std::visit([&](const auto& handle)
						{
							m.Add(handle);
							if constexpr (has_tag_v<std::decay_t<decltype(handle)>::Resource, MetaTag>)
								handle->_dirtymeta = false;
						}, elem);
				}

				Core::GetSystem<FileSystem>().Open(path + ".meta", FS_PERMISSIONS::WRITE) << serialize_text(m);
				resource.is_new = false;
			}
		}
	}

	void ResourceManager::WatchDirectory()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
			Load(elem);
		
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN))
			Load(elem);
	}

	ResourceManager::GeneralLoadResult ResourceManager::Load(PathHandle path, bool reload_resource)
	{
		auto* loader = GetLoader(path.GetExtension());
		if (loader == nullptr)
			return ResourceLoadError::ExtensionNotRegistered;

		if (!path)
			return ResourceLoadError::FileDoesNotExist;

		auto emplace_path = string{ path.GetMountPath() };

		// unload the file if loaded
		{
			auto itr = _loaded_files.find(emplace_path);
			if (itr != _loaded_files.end())
			{
				if (reload_resource)
				{
					// release all resources attached to file
					for (auto& elem : itr->second.bundle.GetAll())
						std::visit([&](const auto& handle) { Release(handle); }, elem);
					_loaded_files.erase(itr);
				}
				else
					return itr->second.bundle;
			}
		}

		// Meta handling
		auto meta_path = PathHandle{ string{path.GetMountPath()} +".meta" };

		auto meta_bundle = [&]() -> opt<MetaBundle>
		{
			if (!meta_path)
				return std::nullopt;

			auto metastream = meta_path.Open(FS_PERMISSIONS::READ, false);
			auto metastr = stringify(metastream);
			try
			{
				return parse_text<MetaBundle>(metastr);
			}
			catch (...)
			{
				return std::nullopt;
			}
		}();

		// reload the file
		auto bundle = [&]()
		{
			if (meta_bundle && meta_bundle->metadatas.size())
				return loader->LoadFile(path, *meta_bundle);
			else
				return loader->LoadFile(path);
		}();
		const auto [itr, success] = _loaded_files.emplace(emplace_path, FileControlBlock{ bundle });
		assert(success);

		itr->second.is_new = !s_cast<bool>(meta_bundle);

		// set path
		for (auto& elem : bundle.GetAll())
			std::visit(
				[&](const auto& handle) 
				{
					using Res = typename std::decay_t<decltype(handle)>::Resource;

					auto* cb = GetControlBlock(handle);
					assert(cb);
					cb->path   = emplace_path;
				}
			, elem);

		return bundle;
	}

	ResourceManager::GeneralGetResult ResourceManager::Get(PathHandle path)
	{
		auto itr = _loaded_files.find(string{ path.GetMountPath() });
		if (itr != _loaded_files.end())
			return itr->second.bundle;
		return BundleGetError::ResourceNotLoaded;
	}

	FileMoveResult ResourceManager::Rename(PathHandle old_path, string_view new_path)
	{
		auto itr = _loaded_files.find(string{ old_path.GetMountPath() });
		if (itr == _loaded_files.end())
			return FileMoveResult::Error_ResourceNotFound;

		if (Core::GetSystem<FileSystem>().Exists(new_path))
			return FileMoveResult::Error_DestinationExists;

		auto string_path = string{ new_path };
		Core::GetSystem<FileSystem>().Rename(old_path.GetMountPath(), string_path);

		auto bundle = std::move(itr->second);
		for (auto& elem : bundle.bundle.GetAll())
		{
			std::visit([&](const auto& handle)
				{
					GetControlBlock(handle)->path = string_path;
				}, elem);
		}

		_loaded_files.erase(itr);
		_loaded_files.emplace(string_path, bundle);


		return FileMoveResult::Ok;
	}
	
}