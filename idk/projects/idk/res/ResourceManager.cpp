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
									if (h && h->IsDirty())
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
			static hash_set<string_view> GenCompilableExtensions()
			{
				return { Rs::ext... };
			}
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

		for(auto& elem : Core::GetSystem<FileSystem>().GetEntries("/build", FS_FILTERS::ALL | FS_FILTERS::FILE))
		{
			if (elem.GetMountPath().starts_with("/build") && elem.IsFile())
				LoadCompiledAsset(elem);
		}
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
		auto meta = PathHandle{ string{path_to_file.GetMountPath()} +".meta" };
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
		static vector<Path> dirty_files;
		for (auto& [path, resource] : _loaded_files)
		{
			auto dirty = resource.is_new; // is the resource brand new?

			if (!dirty) // are any resource metas dirty?
				for (auto& elem : resource.bundle.GetAll())
				{ 
					dirty = std::visit([&](const auto& handle) -> bool
						{
							using Res = typename std::decay_t<decltype(handle)>::Resource;
							if constexpr (has_tag_v<Res, MetaResource>)
								return GetControlBlock(handle)->dirty_meta;
							else
								return false;
						}, elem);
				}
			// if (path.find("YY_model.fbx") != string::npos) __debugbreak();
			if (dirty)
			{
				dirty_files.push_back(path);
			}
		}

		for (auto& path : dirty_files)
		{
			auto& resource = _loaded_files[path];
			// save the .meta file
			MetaBundle m;

			for (auto& elem : resource.bundle.GetAll())
			{
				std::visit([&](const auto& handle)
					{
						m.Add(handle);
						if constexpr (has_tag_v<std::decay_t<decltype(handle)>::Resource, MetaResource>)
							GetControlBlock(handle)->dirty_meta = true;
					}, elem);
			}
			auto test = serialize_text(m);
			{
				auto stream = Core::GetSystem<FileSystem>().Open(path + ".meta", FS_PERMISSIONS::WRITE);
				stream << test;
				stream.close();
				stream.clear();
			}
			resource.is_new = false;
			Load(path, true);
		}

		dirty_files.clear();
	}

	void ResourceManager::WatchDirectory()
	{
		for (auto& elem : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::DELETED))
			Unload(PathHandle{ elem });

		LoadPaths(Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED));
		LoadPaths(Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::WRITTEN));
		Core::GetSystem<Application>().WaitForChildren();
	}

	void ResourceManager::LoadPaths(span<const PathHandle> paths)
	{
		for (auto& elem : paths)
		{
			if (elem.GetMountPath().starts_with("/assets"))
			{
				if (_compilable_extensions.find(elem.GetExtension()) != _compilable_extensions.end())
					LoadAsync(elem);
				else
					Load(elem);
			}
			if (elem.GetMountPath().starts_with("/build"))
				LoadCompiledAsset(elem);
		}
	}

	bool ResourceManager::IsExtensionSupported(string_view ext)
	{
		return _file_loader.find(ext.data()) != _file_loader.end() || _compilable_extensions.find(ext.data()) != _compilable_extensions.end();
	}

	ResourceManager::GeneralLoadResult ResourceManager::Load(PathHandle path, bool reload_resource)
	{
		if (!reload_resource)
		{
			if (auto itr = _loaded_files.find(path.GetMountPath()); itr != _loaded_files.end())
				return itr->second.bundle;
		}
		else
		{
			// unload old data
			auto itr = _loaded_files.find(path.GetMountPath()); 
			if (itr != _loaded_files.end())
			{
				// release all resources attached to file
				for (auto& elem : itr->second.bundle.GetAll())
					elem.visit([&](const auto& handle) { Release(handle); });
				_loaded_files.erase(itr);
			}
		}
		LOG_TO(LogPool::SYS, "Loading file: %s", path.GetMountPath().data());

		auto old_bundle = GetMeta(path);

		auto ext = path.GetExtension();
		if (ext == ".time")
			return {};
		if (path.IsDir())
			return {};
		
		auto emplace_path = string{ path.GetMountPath() };
		auto* loader = GetLoader(ext);

		auto [resource_bundle, meta_bundle] = [&]() -> std::tuple<ResourceBundle, MetaBundle>
		{
			if (ext == ".meta")
			{
				auto meta_bundle = MetaBundle{};
				{	// try to create meta
					auto metastream = path.Open(FS_PERMISSIONS::READ, false);
					auto metastr = stringify(metastream);
					parse_text(metastr, meta_bundle);
				}
				old_bundle = meta_bundle;
				ResourceBundle resource_bundle{meta_bundle};

				auto mount_path = path.GetMountPath();
				emplace_path = string{ mount_path.substr(0, mount_path.length() - string_view(".meta").length()) };
				return std::make_tuple(resource_bundle, meta_bundle);
			}

			if (_compilable_extensions.find(ext) != _compilable_extensions.end())
			{
				LoadAsync(path, true);

				// check compiled results
				auto new_bundle = GetMeta(path);
				if (new_bundle)
				{
					ResourceBundle resource_bundle{ new_bundle };
					for (auto& elem : resource_bundle.GetAll())
					{
						for (auto& _loader : _compiled_asset_loader)
							LoadCompiledAsset(PathHandle{ "/build/" + string{elem.guid()} +string{_loader.first} });
					}
					return std::make_tuple(resource_bundle, new_bundle);
				}
				else
					return {};
			}
			else
			{
				if (loader == nullptr)
				{
					return{};
				}
				auto meta_path = PathHandle{ string{path.GetMountPath()} +".meta" };

				auto meta_bundle = MetaBundle{};
				{	// try to create meta
					auto metastream = meta_path.Open(FS_PERMISSIONS::READ, false);
					auto metastr = stringify(metastream);
					parse_text(metastr, meta_bundle);
				}

				// reload the file
				auto bundle = loader->LoadFile(path, meta_bundle);

				auto new_meta = MetaBundle{};
				for (auto& elem : bundle.GetAll())
					std::visit([&](auto& handle) { new_meta.Add(handle);  }, elem);

				return std::make_tuple(bundle, new_meta);
			}
		}();

		auto& fcb = _loaded_files[emplace_path];
		if (old_bundle != meta_bundle) // meta is invalidated
		{
			fcb.is_new = true;
			for (auto& elem : resource_bundle.GetAll())
				std::visit([](auto& handle) {
				if constexpr (has_tag_v<std::decay_t<decltype(handle)>, MetaResource>)
					handle->DirtyMeta(); },
					elem);
		}

		fcb.bundle = resource_bundle;

		// set path of resources
		for (auto& elem : resource_bundle.GetAll())
			std::visit(
				[&](const auto& handle)
				{
					using Res = typename std::decay_t<decltype(handle)>::Resource;

					auto* cb = GetControlBlock(handle);
					IDK_ASSERT(cb);
					cb->path = emplace_path;
				}
		, elem);

		return resource_bundle;
	}

	void ResourceManager::LoadAsync(PathHandle path, bool wait)
	{
		if (path.GetExtension() == ".meta")
		{
			Load(path);
			return;
		}

		auto last_compiled_file_time = [&]() -> long long
		{
			auto p = PathHandle{ "/build" + string{ path.GetMountPath() } +".time" };
			if (p)
				return p.GetLastWriteTime().time_since_epoch().count();
			return 0;
		}();

		auto meta_path = PathHandle{ string{path.GetMountPath()} +".meta" };
		auto file_is_updated = meta_path ?
			last_compiled_file_time < path.GetLastWriteTime().time_since_epoch().count()
			|| last_compiled_file_time < meta_path.GetLastWriteTime().time_since_epoch().count()
			: true;

		// call compiler
		if (file_is_updated)
		{
			auto wrap = [](string_view str) -> string
			{
				return '\"' + string{ str } +'\"';
			};
			auto infile = wrap(path.GetFullPath());
			auto outdir = wrap(PathHandle{ "/build" }.GetFullPath());
			auto mountdir = wrap(string(PathHandle("/build").GetFullPath()) + string{ path.GetMountPath() });
			const char* exec[] = { infile.data(), outdir.data(), mountdir.data() };
			Core::GetSystem<Application>().Exec(Core::GetSystem<Application>().GetExecutableDir() + "\\tools\\compiler\\idc.exe", exec, wait);
		}
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

	void ResourceManager::RegisterCompilableExtension(string_view ext)
	{
		_compilable_extensions.emplace(string{ ext });
	}
	
}