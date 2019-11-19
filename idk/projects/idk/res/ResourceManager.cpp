#include "stdafx.h"

#include <sstream>

#include <ds/ranged_for.h>
#include <file/FileSystem.h>
#include <res/MetaBundle.h>
#include <serialize/text.h>
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

			template<typename R>
			constexpr static void InitFactoryHC(ResourceManager* resource_man)
			{
				if constexpr (has_extension_v<R>)
					resource_man->_extension_lut.emplace(R::ext, BaseResourceID<R>);

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

			constexpr static auto GenerateExtensionCreateTable()
			{
				return array<void(*)(ResourceManager*, Guid, PathHandle), sizeof...(Rs)>
				{
					[](ResourceManager* rm, Guid guid, PathHandle path)
					{
						rm->GetTable<Rs>()[guid].resource = rm->GetFactory<ResourceFactory<Rs>>().Create(path);
						rm->OnResourceCreate<Rs>().Fire(RscHandle<Rs>{guid});
					}
					...
				};
			}
		};

		using ResourceHelper = ResourceManager_detail<Resources>;
	}

	void ResourceManager::WatchBuildDirectory()
	{
		for (auto& file : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
			if (file.GetMountPath().find("/build") != std::string_view::npos)
				LoadResource(file);
	}

	void ResourceManager::Init()
	{
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

		for (auto& file : Core::GetSystem<FileSystem>().GetEntries("/build", FS_FILTERS::ALL))
		{
			if (file.IsFile())
				LoadResource(file);
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
	}

	void ResourceManager::LoadResource(PathHandle file)
	{
		static constexpr auto load_jt = detail::ResourceHelper::GenerateExtensionCreateTable();

		const auto guid = Guid{ file.GetStem() };
		const auto itr = _extension_lut.find(file.GetExtension());
		if (itr != _extension_lut.end())
			load_jt[itr->second](this, guid, file);
	}
}