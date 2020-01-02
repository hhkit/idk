#include "stdafx.h"

#include <sstream>

#include <ds/ranged_for.inl>
#include <file/FileSystem.h>
#include <res/MetaBundle.inl>
#include <serialize/text.inl>
#include <util/ioutils.h>
#include <res/SaveableResourceLoader.inl>
#include <res/CompiledAssets.h>
#include <res/CompiledAssetLoader.inl>
#include <app/Application.h>

#include <IncludeResources.h>
#include <reflect/reflect.inl>
#include "ResourceHandle.inl"
#include <res/ResourceMeta.inl>
#include "ResourceManager.inl"
#include <scene/Scene.inl>
#include <res/ResourceUtils.inl>
#include <ds/span.inl>
#include <ds/result.inl>

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
			template<typename R>
			constexpr static void InitFactoryHC(ResourceManager* resource_man)
			{
				auto loader = &resource_man->GetFactoryRes<R>();
				if (loader)
					loader->Init();
			}
			constexpr static array<void(*)(ResourceManager*), sizeof...(Rs)> InitFactories()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>{
						&InitFactoryHC<Rs>...
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

		template<typename T>
		struct CompiledAssetHelper_detail;

		template<typename ... Rs>
		struct CompiledAssetHelper_detail<std::variant<Rs...>>
		{
		};

		using AssetHelper = CompiledAssetHelper_detail<CompiledVariant>;
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
		fs.Mount(exe_dir + "/engine_data", "/engine_data");
		fs.Mount(exe_dir + "/build", "/build");
	}

	void ResourceManager::LateInit()
	{
		constexpr static auto init_table = detail::ResourceHelper::InitFactories();
		constexpr static auto defaults_table = detail::ResourceHelper::GenDefaults();
		for (auto& func : init_table)
			func(this);
		for (auto& func : defaults_table)
			func(this);
		SaveDirtyMetadata();
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

	MetaBundle ResourceManager::GetMeta(PathHandle path_to_file)
	{
		auto meta = PathHandle{ string{path_to_file.GetFullPath()} +".meta" };
		auto bundlestream = meta.Open(FS_PERMISSIONS::READ);
		auto res = parse_text<MetaBundle>(stringify(bundlestream));
		return res ? *res : MetaBundle{};
	}

	ResourceManager& ResourceManager::Instance() noexcept
	{
		return *instance;
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
			// if (path.find("YY_model.fbx") != string::npos) __debugbreak();
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
				auto test = serialize_text(m);
				Core::GetSystem<FileSystem>().Open(path + ".meta", FS_PERMISSIONS::WRITE) << test;
				resource.is_new = false;
			}
		}
	}

	void ResourceManager::WatchDirectory()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED))
		{
			Unload(PathHandle{ elem });
		}

		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
			Load(elem);
		
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN))
			Load(elem);
	}

	bool ResourceManager::IsExtensionSupported(string_view ext)
	{
		auto itr = _file_loader.find(ext.data());
		return itr != _file_loader.end();
	}

	ResourceManager::GeneralLoadResult ResourceManager::Load(PathHandle path, bool reload_resource)
	{
		if (!reload_resource)
		{
			if (auto itr = _loaded_files.find(path.GetMountPath()); itr != _loaded_files.end())
				return itr->second.bundle;
		}

		

		auto file_is_updated = [&]() -> bool
		{
			auto file_last_write = path.GetLastWriteTime();
			auto meta = PathHandle{ string{path.GetFullPath()} +".meta" };
			return meta ? meta.GetLastWriteTime() <= file_last_write : true;
		}();

		auto old_bundle = GetMeta(path);

		auto ext = path.GetExtension();
		if (ext == ".meta")
			return {};

		auto* loader = GetLoader(ext);
		if (loader == nullptr)
		{
			if (path.IsFile())
			{
				// call compiler
				if (file_is_updated)
				{
					auto wrap = [](string_view str) -> string
					{
						return '\"' + string{ str } +'\"';
					};
					auto infile = wrap(path.GetFullPath());
					auto outdir = wrap(PathHandle{ "/build" }.GetFullPath());
					const char* exec[] = { infile.data(), outdir.data() };
					Core::GetSystem<Application>().Exec(Core::GetSystem<Application>().GetExecutableDir() + "\\tools\\compiler\\idc.exe", exec, true);
				}

				// check compiled results
				auto new_bundle = GetMeta(path);
				if (new_bundle)
				{
					ResourceBundle resource_bundle;
					for (auto& elem : new_bundle.metadatas)
					{
						GenericResourceHandle{ elem.t_hash, elem.guid }.visit([&](auto& handle)
						{
							resource_bundle.Add(handle);
						});
					}
					return resource_bundle;
				}
			}
		}
		else
		{
			auto new_bundle = loader->LoadFile(path, old_bundle);
			return new_bundle;
		}
		return {};
	}

	void ResourceManager::LoadCompiledAsset(PathHandle path)
	{
		auto itr = _compiled_asset_loader.find(path.GetExtension());
		if (itr != _compiled_asset_loader.end())
			itr->second->LoadAsset(path);
	}

	void ResourceManager::Unload(PathHandle path)
	{
		LOG_TO(LogPool::SYS, "Unloading %s. ", path.GetMountPath().data());
		auto bundle = Get(path);
		if (bundle)
		{
			for (auto& resource : bundle->GetAll())
				std::visit([&](auto& res_handle) {Release(res_handle); }, resource);
		}

		// Remove from loaded files if its there
		auto itr = _loaded_files.find(path.GetMountPath().data());
		if (itr != _loaded_files.end())
		{
			_loaded_files.erase(itr);
		}
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