#include "stdafx.h"

#include <sstream>

#include <ds/ranged_for.h>
#include <file/FileSystem.h>
#include <res/MetaBundle.h>
#include <serialize/text.h>
#include <util/ioutils.h>

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

			constexpr static auto GenerateSignalTable()
			{
				return array<void(*)(ResourceManager*), sizeof...(Rs)>
				{
					[](ResourceManager* rm)
					{
						rm->_created_signals[BaseResourceID<Rs>] = std::make_shared<Signal<RscHandle<Rs>>>();
						rm->_destroying_signals[BaseResourceID<Rs>] = std::make_shared<Signal<RscHandle<Rs>>>();
					}
						...
				};
			}
		};

		using ResourceHelper = ResourceManager_detail<Resources>;
	}

	ResourceManager::~ResourceManager() = default;

	void ResourceManager::WatchBuildDirectory()
	{
		for (auto& file : Core::GetSystem<FileSystem>().QueryFileChangesByChange(FS_CHANGE_STATUS::CREATED))
			if (file.GetMountPath().find("/build") != std::string_view::npos)
				LoadResource(file);
	}

	bool ResourceManager::IsExtensionSupported(string_view ext)
	{
		return _extension_lut.find(ext) != _extension_lut.end();
	}

	void ResourceManager::Init()
	{
		constexpr static auto signal_table = detail::ResourceHelper::GenerateSignalTable();
		instance = this;
		_resource_table = detail::ResourceHelper::GenResourceTables();

		for (auto& func : signal_table)
			func(this);

		auto& fs = Core::GetSystem<FileSystem>();
		auto exe_dir = string{ fs.GetExeDir() };

        fs.Mount(exe_dir + "/engine_data", "/engine_data");
		std::filesystem::create_directory(exe_dir + "/build");
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